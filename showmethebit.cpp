#include <print>
#include <iostream>
#include <array>

constexpr int BYTE_SIZE = 8;

static consteval bool IsBigEndian()
{
	return std::endian::native == std::endian::big;
}

static void PrintChar(char c)
{
	for (int i = BYTE_SIZE * sizeof(char) - 1; i >= 0; --i)
	{
		std::print("{}", ((c >> i) & 1) == 0 ? 0 : 1);
	}
	std::print(" ");
}

template<typename T>
void PrintType(const T& val)
{
	std::println("The binary representation is:");
	if constexpr (IsBigEndian())
	{
		for (int i = 0; i < sizeof(T) / sizeof(char); ++i)
		{
			PrintChar(*(reinterpret_cast<const char*>(&val) + i));
		}
	}
	else
	{
		for (int i = sizeof(T) / sizeof(char) - 1; i >= 0; --i)
		{
			PrintChar(*(reinterpret_cast<const char*>(&val) + i));
		}
	}
	std::println();
}

template<typename T>
inline T TakeValue()
{
	std::println("Then type a value: ");
	T val{};
	std::cin >> val;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	return val;
}

template<typename T>
inline void FetchAndPrint()
{
	PrintType(TakeValue<T>());
	std::println();
}

template<typename... Ts>
constexpr std::array<void(*)(), sizeof...(Ts)> func_array{ &FetchAndPrint<Ts>... };

int main()
{
	std::string_view types[] = {"int", "long", "short", "char", "float", "double", "unsigned int", "unsigned long", "unsigned short", "unsigned char"};
	auto& func_table = func_array<int, long, short, char, float, double, unsigned int, unsigned long, unsigned short, unsigned char>;
	std::print("Choose a type(or -1 to quit):\n");
	for (int i = 1; i <= sizeof(types) / sizeof(std::string_view); ++i)
	{
		std::println("({}) {}", i, types[i - 1]);
	}
	std::println();
	
	while (true)
	{
		int opt;
		std::cin >> opt;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::println("Invalid input, please type again.");
		}
		else if (opt == -1)
			break;
		else if (opt < 1 || opt > func_table.size())
			std::println("Invalid value, please type again.");
		else
		{
			func_table[opt - 1]();
			std::println("Continue(or quit)? (y/n)");
			char choice;
			std::cin >> choice;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			if (choice == 'n')
				break;
			else if (choice == 'y')
				std::println("Choose a type:");
			else
				std::println("Invalid choice, continue to choose a type:");
		}
	}
}
