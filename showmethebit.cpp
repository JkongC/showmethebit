#include <print>
#include <iostream>
#include <array>
#include <source_location>

constexpr int BYTE_SIZE = 8;

// 是否为大端序，这会影响打印的顺序。
static consteval bool IsBigEndian()
{
	return std::endian::native == std::endian::big;
}

// 获取类型的字符串形式，省去手打的功夫。
template<typename T>
constexpr std::string_view GetTypeName()
{
#if defined (_MSC_VER)
	std::string_view whole = std::source_location::current().function_name();
	auto start = whole.find("GetTypeName");
	auto end = whole.rfind(">");
	return std::string_view{ whole.begin() + start + sizeof("GetTypeName"), whole.begin() + end };
#else
	static_assert(false, "This function is not enabled for current compiler.");
#endif
}

static void PrintChar(char c)
{
	for (int i = BYTE_SIZE * sizeof(char) - 1; i >= 0; --i)
	{
		std::print("{}", ((c >> i) & 1) == 0 ? 0 : 1);
	}
	std::print(" ");
}

// 将值转换为char类型以便打印。
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

// 从cin获取值，忽略无效值。
template<typename T>
void SafeInput(T& result)
{
	std::cin >> result;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// 从cin获取值，然后打印。
template<typename T>
inline void FetchAndPrint()
{
	std::println("Then type a value: ");
	T val{};
	SafeInput(val);
	PrintType(val);
	std::println();
}

// 为了简化类型相关操作，避免后续增加类型时徒增工作量。
template<typename... Ts>
struct TypeInfo
{
	static constexpr std::array<std::string_view, sizeof...(Ts)> names{	GetTypeName<Ts>()... };
	static constexpr std::array<void(*)(), sizeof...(Ts)> print_funcs{ &FetchAndPrint<Ts>... };
};

int main()
{
	using types = TypeInfo<int, long, short, char, float, double, unsigned int, unsigned long, unsigned short, unsigned char>;
	std::print("Choose a type(or -1 to quit):\n");
	for (int i = 1; i <= sizeof(types::names) / sizeof(std::string_view); ++i)
	{
		std::println("({}) {}", i, types::names[i - 1]);
	}
	std::println();
	
	while (true)
	{
		int opt;
		SafeInput(opt);

		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::println("Invalid input, please type again.");
		}
		else if (opt == -1)
			break;
		else if (opt < 1 || opt > types::print_funcs.size())
			std::println("Invalid value, please type again.");
		else
		{
			std::print("You chose type \"{}\". ", types::names[opt - 1]);
			// 调用相应类型的打印函数。
			types::print_funcs[opt - 1]();
			std::println("Continue(or quit)? (y/n)");

			char choice;
			SafeInput(choice);

			if (choice == 'n')
				break;
			else if (choice == 'y')
				std::println("Choose a type:");
			else
				std::println("Invalid choice, continue to choose a type:");
		}
	}
}
