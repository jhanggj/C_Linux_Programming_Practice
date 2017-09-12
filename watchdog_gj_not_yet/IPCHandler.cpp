#include "IPCHandler.h"
#include <iostream>
using namespace std;

IPCHandler::IPCHandler():holder(NULL)
{
	int	ret = -1;
	cout << "*** IPCHandler constructer without pra is running ... ***" << endl;

	ipc_fd = ipc_daemon.init(WATCHDOG_SOCKET_NAME);

	cout << "ipc_fd is " << ipc_fd << endl;
	do {
		if (ipc_fd < 0) {
			ERROR("create ipc daemon fail");
			break;
		}
		ret = 0;
	} while(false);

	holder = StatesHolder::CreateStatesHolder();
	if (holder == NULL) {
		fprintf(stderr, "%s: %s[%d] can not alloc holder\n", __FILE__, __FUNCTION__, __LINE__);
	}
}

IPCHandler::~IPCHandler()
{
	cout << "*** IPCHandler destructer is running ... ***" << endl;
	cout << "ipc_fd is " << ipc_fd << endl;
	if (holder) {
		StatesHolder::ReleaseStatesHolder();
		holder = NULL;
	}

	//TODO It should be closed by ipc_daemon.deinit, but ipc_damemon didin't implement this function.
	//close(ipc_fd); <--- need it? ls /proc/$(pidof Watchdog)/fd to watch it.
}

int IPCHandler::init()
{
	int ret = 0;
	/*
	do {
		if (ipc_daemon.init(WATCHDOG_SOCKET_NAME) < 0) {
			ERROR("create ipc daemon fail");
			break;
		}
		ret = 0;
	} while(false);
	led.init();
	holder = StatesHolder::CreateStatesHolder();
	if (!holder) {
		fprintf(stderr, "%s: %s[%d] can not alloc holder\n", __FILE__, __FUNCTION__, __LINE__);
	}
	*/
	return ret;
}

int IPCHandler::release()
{
	/*
	fprintf(stderr, "%s: %s[%d] IPCHandler is released\n", __FILE__, __FUNCTION__, __LINE__);
	if (holder) {
		StatesHolder::releaseStatesHolder();
		holder = NULL;
	}
	*/
	return 0;
}

bool IPCHandler::GoNextState()
{
	//return (GetMainHandlerState() != holder->GetMainStates());
	return 0;
}

int IPCHandler::run_parsing_command()
{
	int client_fd, ret;
	IpcCommand cmd;
	while (!get_quit() && !get_reload() && !GoNextState()) {
		//INFO("%d %d", get_quit(), get_reload());
		ret = ipc_daemon.wait_cmd_timeout(&cmd, sizeof(cmd), client_fd, 0, POLLING_TIMEOUT);
		//ret = ipc_daemon.wait_cmd_timeout(&cmd, sizeof(cmd), client_fd, 0, 1000000);
		if (ret > 0) {
			IpcResponse res = { 0, 0, 0, "" };	// always response with success.
			INFO("\033[1;33m[watchDog]: received command[%d]\033[0m", cmd.id);
			switch (cmd.id) {
				case CMD_DET_FACTORY_BUTTON:
					printf("DET_FACTORY_BUTTON\n");
					res.result = handle_detect_factory_button();
					break;
				case CMD_FACTORY_RESET_ACT: 
					printf("FACTORY_RESET_ACT\n");
					//res.result = handle_factory_reset();
					res.result = 0;
					break;
				/*
				case CMD_FIRMWARE_UPGRADE_START: {
													 Color green(LED_GREEN, BLINK, FIXED_VALUE_FOR_INTERACTIVE, BLINK_PERIOD);
													 Color yellow(LED_YELLOW, BLINK, FIXED_VALUE_FOR_INTERACTIVE, BLINK_PERIOD);
													 led.setBlink(green, yellow);
													 res.result = 0;
												 } break;
				case CMD_ENTER_DAY_MODE:
				case CMD_ENTER_NIGHT_MODE:
				case CMD_STANDBY_LED_UPDATE: {
												 Color c = holder->GetLastColor1();
												 Color n(c.pattern, c.mode, c.interactive, c.period, c.brightness);
												 led.setColor(n);
												 res.result = 0;
											 } break;
				default:
											 res.result = handle_ipc_depend_on_status(cmd);
											 break;
				 */
			}

			ipc_daemon.reply_cmd(client_fd, &res, sizeof(res));
		} else if (ret == 0) {
			/* Do Nothing */		
		}
	}
	return 0;
}

int IPCHandler::handle_detect_factory_button()
{
	LED_STATUS_SETTING led_status_setting;

	/* Should Get First */ 
	printf("Get_status_info_by_type\n");
	holder->get_status_info_by_type("LEDStatus", &led_status_setting);
	
	printf("Set_status_info_by_type\n");
	led_status_setting.pled_state = PLED_RESET;
	holder->set_status_info_by_type("LEDStatus", &led_status_setting);

	return 0;

}

int IPCHandler::handle_factory_reset()
{
	system
		("( /etc/rc.d/init.d/services.sh stop; factoryReset > /dev/null 2> /dev/null; /sbin/reboot ) &");
	
	return 0;
}

int IPCHandler::handle_select_time_out()
{
	return 0;
}

int IPCHandler::handle_network_states()
{
	return 0;
}

