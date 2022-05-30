/**
 * @file Main.cpp
 *
 * @brief Entrypoint of the application.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include "MainWindow.h"

int main()
{
	MainWindow MainWindow;
	int init_value = MainWindow.initialisation();
	if (init_value != 0) {
		// There was a problem during the initialization
		// imediately quit the application
		return init_value;
	}

	return MainWindow.renderLoop();
}