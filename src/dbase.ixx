// interface module for DBase work class

/*
Требуется хранить персональную информацию о клиентах:

имя
фамилия
email
телефон
*/

/*
Необходимо разработать структуру БД для хранения информации и написать класс на С++ для управления данными, со следующими методами:

Метод, создающий структуру БД (таблицы)
Метод, позволяющий добавить нового клиента
Метод, позволяющий добавить телефон для существующего клиента
Метод, позволяющий изменить данные о клиенте
Метод, позволяющий удалить телефон для существующего клиента
Метод, позволяющий удалить существующего клиента
Метод, позволяющий найти клиента по его данным (имени, фамилии, email-у или телефону)
*/

module;

#include <iostream>
#include <vector>
#include <memory>
#include <pqxx/pqxx>

export module dbase;

namespace jinx
{

	export class DBASE final
	{
	public:
		// PUBLIC AREA -----------------------------------

		// public methods

		/*
		Вывод всей БД
		*/
		void print_db();

		/*
		Вывод информации о пользователе
		*/
		void get_user(std::string);

		/*
		Добавить нового клиента
		*/
		void add_client();

		/*
		Добавить телефоны существующему клиенту
		*/
		void add_phone(std::string);

		/*
		Изменить данные пользователя
		*/
		void change_user(std::string);

		/*
		Удалить выбранного пользователя из бд
		*/
		void delete_user(std::string);

		/*
		Удалить телефоны пользователя
		*/
		void remove_phones(std::string);

		/*
		Поиск пользователя по разным параметрам
		*/
		void search(int);

		// overloads

		// constructors

		DBASE() = delete;

		DBASE(std::string, std::string, std::string);

		// destructor

	protected:
		// PROTECTED AREA ---------------------------------

	private:
		// PRIVATE AREA -----------------------------------
		std::string _dbname;
		std::string _dbuser;
		std::string _dbpasswd;

		std::unique_ptr<pqxx::connection> con;

		// private methods

		void make_tables();

	}; // CLASS DBASE ------------------------------------------------------------------------------

	DBASE::DBASE(std::string dbname, std::string dbuser, std::string dbpasswd) : _dbname(dbname), _dbuser(dbuser), _dbpasswd(dbpasswd)
	{
		// test con & create tables if not exist

		con = std::make_unique<pqxx::connection>
			(
			"host=127.0.0.1 "
			"port=5432 "
			"dbname=" + _dbname + " "
			"user=" + _dbuser + " "
			"password=" + _dbpasswd
			);

		con->prepare("insert_user", "insert into dbusers (fname, lname, email) values ($1, $2, $3)");
		con->prepare("insert_phone", "insert into dbphones (userid, phonenum) values($1, $2)");
		con->prepare("change_fname", "update dbusers set fname = $1 where userid = $2;");
		con->prepare("change_lname", "update dbusers set lname = $1 where userid = $2;");
		con->prepare("change_email", "update dbusers set email = $1 where userid = $2;");

		make_tables();

	} // MAIN CONSTRUCTOR FUNC ------------------------------------------------------------------------------

	void DBASE::make_tables()
	{		
		pqxx::work tx1(*con);
		tx1.exec("CREATE table if not exists dbusers (userid serial primary key, fname varchar(60) not null, lname varchar(60) not null, email varchar(60) not null); ");
		tx1.commit();

		pqxx::work tx2(*con);
		tx2.exec("create table if not exists dbphones (userid integer references dbusers(userid) not null, phonenum varchar(60) not null); ");
		tx2.commit();
	} // !make_tables  ------------------------------------------------------------------------------

	void DBASE::add_client()
	{
		// VARS

		std::string fname;
		std::string lname;
		std::string email;

		std::vector<std::string> phones;

		char select;

		bool addPhones = false;

		// BODY
		std::cout << "- Добавление пользователя в базу данных\nВведите данные.\n\n";
		std::cout << "ИМЯ: ";
		std::cin >> fname;
		std::cout << "ФАМИЛИЯ: ";
		std::cin >> lname;
		std::cout << "EMAIL: ";
		std::cin >> email;

		/*
		do
		{

		} while (std::find(email.begin(), email.end(), "@") == email.end());
		*/

		std::cout << "\nДобавить телефон? (Y/N)\n> ";

		do
		{
			std::cin >> select;

			if (select == 'Y' || select == 'y')
			{
				// VARS

				std::string phone;
				bool flag = true;
				// BODY

				addPhones = true;

				while (flag)
				{
					std::cout << "PHONE: ";
					std::cin >> phone;
					phones.push_back(phone);

					std::cout << " Добавить ещё телефон?\n> ";

					do
					{
						std::cin >> select;

						if (select == 'Y' || select == 'y')
						{
							flag = true;
						}
						else if (select == 'N' || select == 'n')
						{
							flag = false;
						}
					} while (select != 'Y' && select != 'y' && select != 'N' && select != 'n');
				} // !while (flag)
			} // !if (select == 'Y' || select == 'y')
			else
			{
				// skip adding phone
			}

		} while (select != 'Y' && select != 'y' && select != 'N' && select != 'n');

		// ADDING TO TABLE

		pqxx::work tx1(*con);

		// tx1.exec("insert into dbusers (fname, lname, email) values ('" + fname + "', '" + lname + "', '" + email + "');");
		tx1.exec_prepared("insert_user", fname, lname, email);
		tx1.commit();

		// if phones are added

		if (addPhones)
		{
			pqxx::work tx2(*con);
			std::string userid;

			userid = tx2.query_value<std::string>("select userid from dbusers where fname = '" + fname + "' AND lname = '" + lname + "' AND email = '" + email + "'");

			tx2.abort();

			for (const auto& val : phones)
			{
				pqxx::work tx3(*con);
				// tx3.exec("insert into dbphones (userid, phonenum) values('" + userid + "', '" + val + "');");
				tx3.exec_prepared("insert_phone", userid, val);
				tx3.commit();
			}

		} // if (addPhones)

		std::cout << "\nОперация выполнена!\n";
		std::system("pause");

	} // !add_client() ------------------------------------------------------------------------------

	/*
	Добавить телефоны существующему клиенту
	*/
	void DBASE::add_phone(std::string userid)
	{
		this->get_user(userid);
		std::cout << '\n';

		bool addPhones = false;
		char select;
		std::vector<std::string> phones;

		std::cout << "\nДобавить телефон? (Y/N)\n> ";

		do
		{
			std::cin >> select;

			if (select == 'Y' || select == 'y')
			{
				// VARS

				std::string phone;
				bool flag = true;
				// BODY

				addPhones = true;

				while (flag)
				{
					std::cout << "PHONE: ";
					std::cin >> phone;
					phones.push_back(phone);

					std::cout << " Добавить ещё телефон?\n> ";

					do
					{
						std::cin >> select;

						if (select == 'Y' || select == 'y')
						{
							flag = true;
						}
						else if (select == 'N' || select == 'n')
						{
							flag = false;
						}
					} while (select != 'Y' && select != 'y' && select != 'N' && select != 'n');
				} // !while (flag)
			} // !if (select == 'Y' || select == 'y')
			else
			{
				// skip adding phone
			}

		} while (select != 'Y' && select != 'y' && select != 'N' && select != 'n');

		// ADDING TO TABLE

		// if phones are added

		if (addPhones)
		{
			for (const auto& val : phones)
			{
				pqxx::work tx(*con);
				tx.exec_prepared("insert_phone", userid, val);
				tx.commit();
			}
		} // if (addPhones)	

		std::cout << "\nОперация выполнена!\n";
		std::system("pause");

	} //!add_phone ------------------------------------------------------------------------------

	/*
		Вывод всей БД
	*/
	void DBASE::print_db()
	{
		pqxx::work tx(*con);
		std::cout << "| ID |\t ИМЯ \t|\t ФАМИЛИЯ \t|\t EMAIL \t|\tТЕЛЕФОНЫ (Опц) \t\n";

		for (auto [id, fname, lname, email] : tx.query<std::string, std::string, std::string, std::string>("select userid, fname, lname, email from dbusers	order by userid asc;"))
		{
			std::cout << "  " << id << " \t" << fname << "\t\t" << lname << "\t\t" << email << "\t\t";
			for (auto phone : tx.query<std::string>("select phonenum from dbphones where userid=" + id))
			{
				std::cout << ' ' << std::get<0>(phone) << ' ';
			}
			std::cout << '\n';
		}
		std::cout << "\n|____|________________________________________________\n\n";
	} // !print_db ------------------------------------------------------------------------------

	/*
	Вывод информации о пользователе
	*/
	void DBASE::get_user(std::string userid)
	{
		pqxx::work tx(*con);
		std::cout << "ВЫБРАН ПОЛЬЗОВАТЕЛЬ: \n";
		std::cout << "| ID |\t ИМЯ \t|\t ФАМИЛИЯ \t|\t ТЕЛЕФОНЫ (Опц) \t\n";

		for (auto [id, fname, lname, email] : tx.query<std::string, std::string, std::string, std::string>("select userid, fname, lname, email from dbusers where userid =" + userid + " order by userid asc"))
		{
			std::cout << " " << id << " \t" << fname << "\t\t" << lname << "\t\t" << email << "\t\t";
			for (auto phone : tx.query<std::string>("select phonenum from dbphones where userid=" + id))
			{
				std::cout << ' ' << std::get<0>(phone) << ' ';
			}
		}
		std::cout << "\n|____|________________________________________________\n\n";
	} // !get_user ------------------------------------------------------------------------------

	/*
	Изменить данные пользователя
	*/
	void DBASE::change_user(std::string userid)
	{
		this->get_user(userid);
		std::cout << '\n';
		int selector;
		std::string userInput;		

		std::cout << "Выберите что необходимо изменить.\n";
		std::cout << " - 1) Имя\n"
			" - 2) Фамилия\n"
			" - 3) Почта\n"
			" - 4) Заменить телефоны\n"
			" - 0) Выход и отмена\n";
		std::cout << " Выберите действие (1-4, 0 - отмена): ";
		std::cin >> selector;

		pqxx::work tx(*con);

		switch (selector)
		{
		case 1:
			// change fname
			std::cout << "Введите новое ИМЯ: ";
			std::cin >> userInput;
			tx.exec_prepared("change_fname", userInput, userid);
			tx.commit();
			break;
		case 2:
			// change lname
			std::cout << "Введите новую ФАМИЛИЮ: ";
			std::cin >> userInput;
			tx.exec_prepared("change_lname", userInput, userid);
			tx.commit();
			break;
		case 3:
			// change email
			std::cout << "Введите новый EMAIL: ";
			std::cin >> userInput;
			tx.exec_prepared("change_email", userInput, userid);
			tx.commit();
			break;
		case 4:
			// remove and add new phones
			tx.exec("delete from dbphones where userid=" + userid);
			tx.commit();
			add_phone(userid);
			break;
		case 0:
			// do nothing and exit
			break;
		default:
			std::cout << "\n - Unforseen Consequenes - \n";
			break;			
		}

		std::cout << "\nОперация выполнена!\n";
		std::system("pause");
	} // !change_user ------------------------------------------------------------------------------

	/*
	Удалить выбранного пользователя из бд
	*/
	void DBASE::delete_user(std::string userid)
	{
		this->get_user(userid);
		std::cout << '\n';
		char select;
		std::cout << "\nДействительно удалить выбранного пользователя? (Y/N)\n> ";
		
		do
		{
			std::cin >> select;

			if (select == 'Y' || select == 'y')
			{
				pqxx::work tx(*con);

				// remove and add new phones
				tx.exec("delete from dbphones where userid=" + userid);
				tx.exec("delete from dbusers where userid=" + userid);
				tx.commit();				
			} // !if (select == 'Y' || select == 'y')
			else
			{
				// skip adding phone
			}

		} while (select != 'Y' && select != 'y' && select != 'N' && select != 'n');

		std::cout << "\nОперация выполнена!\n";
		std::system("pause");
	} // !delete_user ------------------------------------------------------------------------------


	/*
	Удалить телефоны пользователя
	*/
	void DBASE::remove_phones(std::string userid)
	{
		this->get_user(userid);
		std::cout << '\n';
		char select;
		std::cout << "\nДействительно удалить телефоны выбранного пользователя? (Y/N)\n> ";

		do
		{
			std::cin >> select;

			if (select == 'Y' || select == 'y')
			{
				pqxx::work tx(*con);

				// remove and add new phones
				tx.exec("delete from dbphones where userid=" + userid);
				tx.commit();
			} // !if (select == 'Y' || select == 'y')
			else
			{
				// skip adding phone
			}
		} while (select != 'Y' && select != 'y' && select != 'N' && select != 'n');

		std::cout << "\nОперация выполнена!\n";
		std::system("pause");
			
	} // !remove_phones ------------------------------------------------------------------------------

	/*
	Поиск пользователя по разным параметрам
	*/
	void DBASE::search(int searchmode)
	{
		std::string keyword;
		std::cout << "\nВведите ключ поиска: ";
		std::cin >> keyword;		

		pqxx::work tx(*con);

		std::cout << "Результаты поиска:\n\n";

		std::cout << "| ID |\t ИМЯ \t|\t ФАМИЛИЯ \t|\t ТЕЛЕФОНЫ (Опц) \t\n";

		if (searchmode == 1)
		{		

			for (auto [id, fname, lname, email] : tx.query<std::string, std::string, std::string, std::string>("select userid, fname, lname, email from dbusers where fname ='" + keyword + "' order by userid asc"))
			{
				std::cout << " " << id << " \t" << fname << "\t\t" << lname << "\t\t";

				for (auto phone : tx.query<std::string>("select phonenum from dbphones where userid=" + id))
				{
					std::cout << ' ' << std::get<0>(phone) << ' ';
				}
				std::cout << '\n';
			}			

		} // !searchmode == 1
		else if (searchmode == 2)
		{

			for (auto [id, fname, lname, email] : tx.query<std::string, std::string, std::string, std::string>("select userid, fname, lname, email from dbusers where lname ='" + keyword + "' order by userid asc"))
			{
				std::cout << " " << id << " \t" << fname << "\t\t" << lname << "\t\t";

				for (auto phone : tx.query<std::string>("select phonenum from dbphones where userid=" + id))
				{
					std::cout << ' ' << std::get<0>(phone) << ' ';
				}
				std::cout << '\n';
			}

		} // !searchmode == 2
		else if (searchmode == 3)
		{

			for (auto [id, fname, lname, email] : tx.query<std::string, std::string, std::string, std::string>("select userid, fname, lname, email from dbusers where email ='" + keyword + "' order by userid asc"))
			{
				std::cout << " " << id << " \t" << fname << "\t\t" << lname << "\t\t";

				for (auto phone : tx.query<std::string>("select phonenum from dbphones where userid=" + id))
				{
					std::cout << ' ' << std::get<0>(phone) << ' ';
				}
				std::cout << '\n';
			}

		} // !searchmode == 3

		std::cout << "\n|____|________________________________________________\n\n";

		std::cout << "\nОперация выполнена!\n";
		std::system("pause");

	} // !search() ------------------------------------------------------------------------------------------------------------------------------

} // !JINX