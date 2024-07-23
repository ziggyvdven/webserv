#include <iostream>
#include <fstream>

struct Record {
	char			name[40];
	unsigned short	age;
	float			gpa;
};

int main()
{

	std::fstream f("records.dat", std::ios::binary | std::ios::out);
	Record records[3] = {
		{"Karim", 30, 3.8},
		{"Hank", 40, 3.8},
		{"Same", 28, 2.9}
		};


	if (f)
	{
		f.write(reinterpret_cast<char*>(&records), 3 * sizeof(Record));
		f.close();
	}
	else
	{
		std::cout << "Failed to open file" << std::endl;
	}


	Record records2[3];

	f.open("records.dat", std::ios::in | std::ios::binary);
	if (f)
	{
		f.read(reinterpret_cast<char *>(&records2), 3 * sizeof(Record));

		for (int i = 0; i < 3; i++)
		{
			Record rec2 = records2[i];
			std::cout << rec2.name << "\n" << rec2.age << "\n" << rec2.gpa << std::endl;
		}

		f.close();
	} else {
		std::cout << "File not found" << std::endl;
	}
}