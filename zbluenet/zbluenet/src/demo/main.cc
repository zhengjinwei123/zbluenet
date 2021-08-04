#include "server_app.h"

int main(int argc, char *argv[])
{
	ServerApp::getInstance()->start();
	return 0;
}