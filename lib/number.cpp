#include "number.h"
#include <string>

uint239_t FromInt(uint32_t value, uint32_t shift)
{
	uint239_t result = {};

	int bite_shift = shift % 245;		 // общий сдвиг
	int short_shift = bite_shift % 7; // сдвиг внутри одного байта
	int byte_shift = bite_shift / 7;	 // сдвиг между байтами

	for (int i = 0; i < 5; ++i)
	{
		uint8_t data_piece = (value >> (i * 7)) & 0x7F;

		if (short_shift != 0)
		{
			uint8_t first_part = data_piece >> (7 - short_shift);
			uint8_t second_part = data_piece & ((1 << (7 - short_shift)) - 1);

			result.data[(34 - (i + byte_shift + 1) + 35) % 35] |= first_part;

			result.data[(34 - (i + byte_shift) + 35) % 35] |= second_part << short_shift;
		}
		else
		{
			result.data[(34 - (i + byte_shift) + 35) % 35] |= data_piece;
		}
	}

	for (int i = 0; i < 35; ++i)
	{
		result.data[34 - i] |= (shift & 1) << 7;
		shift >>= 1;
	}

	return result;
}

uint239_t FromString(const char *str, uint32_t shift)
{
	uint239_t result = {};
	uint64_t value = std::stoull(str); // гарантируется, что число в строке < 2^64

	int bite_shift = shift % 245;
	int short_shift = bite_shift % 7;
	int byte_shift = bite_shift / 7;

	for (int i = 0; i < 10; ++i)
	{
		uint8_t data_piece = (value >> (i * 7)) & 0x7F;

		if (short_shift != 0)
		{
			uint8_t first_part = data_piece >> (7 - short_shift);
			uint8_t second_part = data_piece & ((1 << (7 - short_shift)) - 1);

			result.data[(34 - (i + byte_shift + 1) + 35) % 35] |= first_part;

			result.data[(34 - (i + byte_shift) + 35) % 35] |= second_part << short_shift;
		}
		else
		{
			result.data[(34 - (i + byte_shift) + 35) % 35] |= data_piece;
		}
	}

	for (int i = 0; i < 35; ++i)
	{
		result.data[34 - i] |= (shift & 1) << 7;
		shift >>= 1;
	}

	return result;
}

uint239_t operator+(const uint239_t &lhs, const uint239_t &rhs)
{
	uint239_t result = {};

	uint64_t lhs_shift = GetShift(lhs);
	uint64_t rhs_shift = GetShift(rhs);

	uint64_t result_shift = lhs_shift + rhs_shift;
	result_shift %= (1ULL << 35);

	uint239_t lhs_normalized = NormalizeShift(lhs);
	uint239_t rhs_normalized = NormalizeShift(rhs);

	uint8_t carry = 0;

	for (int i = 0; i < 35; ++i)
	{
		uint8_t lhs_byte = lhs_normalized.data[34 - i];
		uint8_t rhs_byte = rhs_normalized.data[34 - i];

		uint16_t sum = lhs_byte + rhs_byte + carry;
		carry = (sum >> 7) & 1;
		sum &= 0x7F;

		int total_bite_shift = result_shift % 245;
		int total_byte_shift = total_bite_shift / 7;
		int total_short_shift = total_bite_shift % 7;

		if (total_short_shift != 0)
		{
			uint8_t first_part = sum >> (7 - total_short_shift);
			uint8_t second_part = sum & ((1 << (7 - total_short_shift)) - 1);

			result.data[(34 - (i + total_byte_shift + 1) + 35) % 35] |= first_part;

			result.data[(34 - (i + total_byte_shift) + 35) % 35] |= second_part << total_short_shift;
		}
		else
		{
			result.data[(34 - (i + total_byte_shift) + 35) % 35] |= sum;
		}
	}

	uint64_t shift_copy = result_shift;
	for (int i = 0; i < 35; ++i)
	{
		result.data[34 - i] |= (shift_copy & 1) << 7;
		shift_copy >>= 1;
	}

	return result;
}

uint239_t operator-(const uint239_t &lhs, const uint239_t &rhs)
{
	uint239_t result = {};

	uint64_t lhs_shift = GetShift(lhs);
	uint64_t rhs_shift = GetShift(rhs);

	uint64_t result_shift = lhs_shift - rhs_shift;
	result_shift = (result_shift + (1ULL << 35)) % (1ULL << 35);

	uint239_t lhs_normalized = NormalizeShift(lhs);
	uint239_t rhs_normalized = NormalizeShift(rhs);

	uint8_t borrow = 0;

	for (int i = 0; i < 35; ++i)
	{
		uint8_t lhs_byte = lhs_normalized.data[34 - i];
		uint8_t rhs_byte = rhs_normalized.data[34 - i];

		int16_t difference = lhs_byte - rhs_byte - borrow;

		if (difference < 0)
		{
			difference += 0x80; // Добавляем 128 (0x80), чтобы получить положительное значение
			borrow = 1;			  // Устанавливаем флаг заимствования
		}
		else
		{
			borrow = 0;
		}

		difference &= 0x7F;

		int total_bite_shift = result_shift % 245;
		int total_byte_shift = total_bite_shift / 7;
		int total_short_shift = total_bite_shift % 7;

		if (total_short_shift != 0)
		{
			uint8_t first_part = difference >> (7 - total_short_shift);
			uint8_t second_part = difference & ((1 << (7 - total_short_shift)) - 1);

			result.data[(34 - (i + total_byte_shift + 1) + 35) % 35] |= first_part;

			result.data[(34 - (i + total_byte_shift) + 35) % 35] |= second_part << total_short_shift;
		}
		else
		{
			result.data[(34 - (i + total_byte_shift) + 35) % 35] |= difference;
		}
	}

	uint64_t shift_copy = result_shift;
	for (int i = 0; i < 35; ++i)
	{
		result.data[34 - i] |= (shift_copy & 1) << 7;
		shift_copy >>= 1;
	}

	return result;
}

uint239_t operator*(const uint239_t &lhs, const uint239_t &rhs)
{
	uint239_t result = {};
	return result;
}

uint239_t operator/(const uint239_t &lhs, const uint239_t &rhs)
{
	uint239_t result = {};
	return result;
}

bool operator==(const uint239_t &lhs, const uint239_t &rhs)
{
	uint239_t lhs_normalized = NormalizeShift(lhs);
	uint239_t rhs_normalized = NormalizeShift(rhs);

	for (int i = 0; i < 35; ++i)
	{
		if (lhs_normalized.data[34 - i] != rhs_normalized.data[34 - i])
		{
			return false;
		}
	}

	return true;
}

bool operator!=(const uint239_t &lhs, const uint239_t &rhs)
{
	return !(lhs == rhs);
}

std::ostream &operator<<(std::ostream &stream, const uint239_t &value)
{
	uint64_t result = 0;
	uint239_t number_normalized = NormalizeShift(value);

	for (int i = 0; i < 35; ++i)
	{
		result |= static_cast<uint64_t>(number_normalized.data[34 - i] & 0x7F) << (i * 7);
	}

	stream << result;
	return stream;
}

uint64_t GetShift(const uint239_t &value)
{
	uint64_t shift = 0;
	for (int i = 0; i < 35; ++i)
	{
		shift |= static_cast<uint64_t>((value.data[34 - i] >> 7) & 1) << i;
	}
	return shift;
}

void PrintBinary(const uint239_t &value)
{
	for (int i = 34; i >= 0; --i)
	{
		uint8_t byte = value.data[34 - i];
		for (int j = 7; j >= 0; --j)
		{
			std::cout << ((byte >> j) & 1);
		}
		std::cout << ' ';
	}
	std::cout << std::endl;
}

uint239_t NormalizeShift(const uint239_t &number)
{
	uint239_t normalized = {};
	uint64_t number_shift = GetShift(number);

	int bite_shift = number_shift % 245;
	int byte_shift = bite_shift / 7;
	int short_shift = bite_shift % 7;

	for (int i = 0; i < 35; ++i)
	{
		uint8_t current_byte = 0;

		if (short_shift != 0)
		{
			int index1 = (34 - (i + byte_shift + 1) + 35) % 35;
			int index2 = (34 - (i + byte_shift) + 35) % 35;
			uint8_t left_part = (number.data[index1] & ((1 << short_shift) - 1)) << (7 - short_shift);
			uint8_t right_part = (number.data[index2] >> short_shift) & ((1 << (7 - short_shift)) - 1);
			current_byte = left_part + right_part;
		}
		else
		{
			int index = (34 - (i + byte_shift) + 35) % 35;
			current_byte = number.data[index] & 0x7F;
		}

		normalized.data[34 - i] = current_byte;
	}

	return normalized;
}

// uint239_t ApplyShift(const uint239_t &number, uint64_t shift)
// {
// 	uint239_t shifted = {};

// 	int bite_shift = shift % 245;
// 	int byte_shift = bite_shift / 7;
// 	int short_shift = bite_shift % 7;

// 	uint8_t carry = 0;

// 	for (int i = 0; i < 35; ++i)
// 	{
// 		uint8_t current_byte = number.data[34 - i] & 0x7F;

// 		if (short_shift != 0)
// 		{
// 			uint8_t shifted_part = (current_byte << short_shift) & 0x7F;
// 			uint8_t carry_part = (current_byte >> (7 - short_shift)) & ((1 << short_shift) - 1);

// 			shifted.data[(34 - (i + byte_shift)) % 35] |= shifted_part;
// 			shifted.data[(34 - (i + byte_shift + 1)) % 35] |= carry_part;
// 		}
// 		else
// 		{
// 			shifted.data[(34 - (i + byte_shift)) % 35] |= current_byte;
// 		}
// 	}

// 	return shifted;
// }