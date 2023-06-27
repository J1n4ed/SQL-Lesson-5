// main file for Dbase work

#include <iostream>
#include <Windows.h>

import dbase;

// UTILITY FUNCTIONS

void clear_screen();
int draw_selector();

int main(int argc, char* argv[])
{
	// SETUP

	setlocale(LC_ALL, "ru_RU.UTF-8");

	// SetConsoleCP(1251);
	// SetConsoleOutputCP(1251);

	// VARIABLES

	std::string dbname;
	std::string dbuser;
	std::string dbpasswd;
	bool cmdArgs = false;
	bool isInitSuccess = false;

	// BODY

	std::cout << " ///////////////// ПОДКЛЮЧЕНИЕ К БАЗЕ ДАННЫХ ///////////////// ";

	std::cout << "\n> Запущенно как: ";

	// Вызов должен быть DBaseWork.exe -db <DB NAME> -u <USER NAME> -p <PWD>, порядок аргументов важен

	for (int i = 0; i < argc; ++i)
	{
		std::cout << argv[i] << ' ';		
	}

	std::cout << "\n\n";

	if (argc == 7)
	{
		// checks ------------

		bool check = false;

		if (strcmp(argv[1], "-db") == 0)
		{
			check = true;
		}

		cmdArgs = check;
		check = false;

		if (strcmp(argv[3], "-u") == 0)
		{
			check = true;
		}

		cmdArgs = check;
		check = false;

		if (strcmp(argv[5], "-p") == 0)
		{
			check = true;
		}

		cmdArgs = check;
		check = false;

		// !checks ---------

		if (cmdArgs)
		{

			dbname = argv[2];
			dbuser = argv[4];
			dbpasswd = argv[6];

		}
	}
	else
	{
		cmdArgs = false;
	}

	while (!isInitSuccess)
	{
		std::cout << "\n\n";

		// Manual input if not cmd args (or invalid args)
		if (!cmdArgs)
		{
			std::cout << "> ИМЯ БД: ";
			std::cin >> dbname;
			std::cout << "> ПОЛЬЗОВАТЕЛЬ: ";
			std::cin >> dbuser;
			std::cout << "> ПАРОЛЬ: ";
			std::cin >> dbpasswd;
		}

		std::cout << "\n> Присоединяемся к базе: " << dbname << " как пользователь: " << dbuser << '\n';

		try
		{
			jinx::DBASE dbase(dbname, dbuser, dbpasswd);

			// MAIN LOOP	

			bool exitFlag = false; // flag indicates exit

			while (!exitFlag)
			{
				clear_screen();

				std::cout << " ///////////////// РАБОТА С БД " + dbname + " ///////////////// ";

				std::cout << "\n> Запущенно как: ";

				// Вызов должен быть DBaseWork.exe -db <DB NAME> -u <USER NAME> -p <PWD>, порядок аргументов важен

				for (int i = 0; i < argc; ++i)
				{
					if (i != argc - 1)
						std::cout << argv[i] << ' ';
					else
						std::cout << "<PWD>";
				}

				std::cout << std::endl << std::endl;
				dbase.print_db();
				std::cout << std::endl << std::endl;

				int action = draw_selector();				

				std::string lookupid;
				int mod = -1;

				switch (action)
				{
				case 1:
					// add client
					std::cout << "  > Добавление нововго клиента:\n";
					dbase.add_client();

					break;
				case 2:
					// add phone to client
					std::cout << "  > Добавить телефон клиенту (по ID):\n";
					std::cout << "Введите ID пользователя: ";
					std::cin >> lookupid;
					dbase.add_phone(lookupid);
					break;
				case 3:
					// change client data
					std::cout << "  > Изменить данные клиента:\n";
					std::cout << "Введите ID пользователя: ";
					std::cin >> lookupid;
					dbase.change_user(lookupid);
					break;
				case 4:
					// remove phone from exist client
					std::cout << " > Удаление телефона текущего клиента:\n";
					std::cout << "Введите ID пользователя: ";
					std::cin >> lookupid;
					dbase.remove_phones(lookupid);
					break;
				case 5:
					// remove client from database
					std::cout << " > Удаление клиента из базы данных:\n";
					std::cout << "Введите ID пользователя: ";
					std::cin >> lookupid;
					dbase.delete_user(lookupid);
					break;
				case 6:
					// find client by params					

					std::cout << " > Поиск пользователей по параметрам, выберите параметр поиска:\n";
					std::cout << "Парметры:\n"
						" > 1) Поиск по имени\n"
						" > 2) Поиск по фамилии\n"
						" > 3) Поиск по почте\n"
						" > 0) Поиск по имени\n";
					std::cout << "Выбор действия: ";
					std::cin >> mod;
					
					switch (mod)
					{
					case (1):
						// search mode = 1, look by fname
						dbase.search(mod);
						break;
					case (2):
						// search mode = 2, look by lname
						dbase.search(mod);
						break;
					case (3):
						// search mode = 3, look by email
						dbase.search(mod);
						break;
					case (0):
						// do nothing and exit
						break;
					default:
						std::cout << "\n - Unforseen Consequenes - \n";
						break;
					}
					break;
				case 0:
					// exit program
					exitFlag = true;
					break;
				default:
					std::cout << "\n - Unforseen Consequenes - \n";
					break;
				}

				if (!exitFlag)
				{
					std::cout << "\nОперация выполнена!\n";
					std::system("pause");
				}

				// to default state
				action = -1;

			} // !EXIT FLAG

			// EXIT

			isInitSuccess = true;
		}
		catch (const std::exception& e)
		{
			std::cout << "\nВозникла проблема при работе с базой данных!\nПричина: " << e.what() << std::endl;
			isInitSuccess = false;
			cmdArgs = false;
		}

	} // while (!isInitSuccess)	

	// EXIT
	std::cout << std::endl;
	system("pause");
	return EXIT_SUCCESS;
} // !MAIN

// Utility

void clear_screen()
{

#ifdef _WIN32
	std::system("cls");
#else
	// Assume POSIX
	std::system("clear");
#endif
}

int draw_selector()
{
	std::cout << 
		"      Доступные операции: \n\n"
		" - 1) Добавить клиента\n"
		" - 2) Добавить телефон для клиента\n"
		" - 3) Изменить данные о клиенте\n"
		" - 4) Удалить телефон для существующего клиента\n"
		" - 5) Удалить существующего клиента\n"
		" - 6) Найти клиента по его данным (имени, фамилии, email - у или телефону)\n"
		" - 0) Завершить работу с базой";

	int action = -1;

	std::cout << "\n\n";

	do
	{
		std::cout << "> Выберите действие: ";
		std::cin >> action;
	} while (action != 1 && action != 2 && action != 3 && action != 4 && action != 5 && action != 6 && action != 0);

	return action;
}