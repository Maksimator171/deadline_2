#include <lib/number.h>
#include <iostream>

int main()
{
	uint239_t value1 = FromString("239", 99);
	uint239_t value2 = FromString("1", 0);

	std::cout << "Shift1: " << GetShift(value1) << std::endl;
	std::cout << "Value1: " << value1 << std::endl;
	std::cout << "Binary1: ";
	PrintBinary(value1);
	std::cout << "Normalize1: " << NormalizeShift(value1) << std::endl;
	std::cout << "BinaryNormalize1: ";
	PrintBinary(NormalizeShift(value1));

	std::cout << "Shift2: " << GetShift(value2) << std::endl;
	std::cout << "Value2: " << value2 << std::endl;
	std::cout << "Binary2: ";
	PrintBinary(value2);
	std::cout << "Normalize2: " << NormalizeShift(value2) << std::endl;
	std::cout << "BinaryNormalize2: ";
	PrintBinary(NormalizeShift(value2));

	std::cout << "IsEqual: " << (value1 == value2) << std::endl;

	std::cout << "Sum: " << value1 + value2 << std::endl;

	return 0;
}