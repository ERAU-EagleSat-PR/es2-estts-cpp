#include <iostream>
#include <string>
#include <array>
#include <sstream>
using namespace std;

string convertString(string input, int arrIndex) {
    stringstream ret;
    ret << input[arrIndex] << input[arrIndex + 1] << input[arrIndex + 2] << input[arrIndex + 3];
    return ret.str();
}

string scrambleHex(string hexArray = nullptr) {
    string newHexArray;
    if (hexArray.length() <= 77) {
        //int* binaryArray = static_cast<int *>(malloc(sizeof(int) * 4 * hexArray.length()));
        int binaryArray[hexArray.length()*4];
        try {
            for (int i = 0; i < hexArray.length() * 4; i += 4) {
                switch (toupper(hexArray[i / 4]))
                {
                    case '0':
                        binaryArray[i] = 0;
                        binaryArray[i + 1] = 0;
                        binaryArray[i + 2] = 0;
                        binaryArray[i + 3] = 0;
                        break;
                    case '1':
                        binaryArray[i] = 0;
                        binaryArray[i + 1] = 0;
                        binaryArray[i + 2] = 0;
                        binaryArray[i + 3] = 1;
                        break;
                    case '2':
                        binaryArray[i] = 0;
                        binaryArray[i + 1] = 0;
                        binaryArray[i + 2] = 1;
                        binaryArray[i + 3] = 0;
                        break;
                    case '3':
                        binaryArray[i] = 0;
                        binaryArray[i + 1] = 0;
                        binaryArray[i + 2] = 1;
                        binaryArray[i + 3] = 1;
                        break;
                    case '4':
                        binaryArray[i] = 0;
                        binaryArray[i + 1] = 1;
                        binaryArray[i + 2] = 0;
                        binaryArray[i + 3] = 0;
                        break;
                    case '5':
                        binaryArray[i] = 0;
                        binaryArray[i + 1] = 1;
                        binaryArray[i + 2] = 0;
                        binaryArray[i + 3] = 1;
                        break;
                    case '6':
                        binaryArray[i] = 0;
                        binaryArray[i + 1] = 1;
                        binaryArray[i + 2] = 1;
                        binaryArray[i + 3] = 0;
                        break;
                    case '7':
                        binaryArray[i] = 0;
                        binaryArray[i + 1] = 1;
                        binaryArray[i + 2] = 1;
                        binaryArray[i + 3] = 1;
                        break;
                    case '8':
                        binaryArray[i] = 1;
                        binaryArray[i + 1] = 0;
                        binaryArray[i + 2] = 0;
                        binaryArray[i + 3] = 0;
                        i += 4;
                        break;
                    case '9':
                        binaryArray[i] = 1;
                        binaryArray[i + 1] = 0;
                        binaryArray[i + 2] = 0;
                        binaryArray[i + 3] = 1;
                        break;
                    case 'A':
                        binaryArray[i] = 1;
                        binaryArray[i + 1] = 0;
                        binaryArray[i + 2] = 1;
                        binaryArray[i + 3] = 0;
                        break;
                    case 'B':
                        binaryArray[i] = 1;
                        binaryArray[i + 1] = 0;
                        binaryArray[i + 2] = 1;
                        binaryArray[i + 3] = 1;
                        break;
                    case 'C':
                        binaryArray[i] = 1;
                        binaryArray[i + 1] = 1;
                        binaryArray[i + 2] = 0;
                        binaryArray[i + 3] = 0;
                        break;
                    case 'D':
                        binaryArray[i] = 1;
                        binaryArray[i + 1] = 1;
                        binaryArray[i + 2] = 0;
                        binaryArray[i + 3] = 1;
                        break;
                    case 'E':
                        binaryArray[i] = 1;
                        binaryArray[i + 1] = 1;
                        binaryArray[i + 2] = 1;
                        binaryArray[i + 3] = 0;
                        break;
                    case 'F':
                        binaryArray[i] = 1;
                        binaryArray[i + 1] = 1;
                        binaryArray[i + 2] = 1;
                        binaryArray[i + 3] = 1;
                        break;
                    default:
                        throw("Error: Not a hex array");
                        break;
                }

                //Scramble by the polynomial 1 + x^12 + x^17

                if (i + 1 >= 12) {
                    if (i + 1 >= 17) {
                        binaryArray[i] = binaryArray[i] ^ (binaryArray[i - 12] ^ binaryArray[i - 17]);
                    }
                    else if (i + 1 < 17) {
                        binaryArray[i] = binaryArray[i] ^ (binaryArray[i - 12] ^ 0);
                    }
                }
                else if (i + 1 < 12) {
                    binaryArray[i] = binaryArray[i] ^ 0;
                }

                // End of the for
            }
        }
        catch (string errorMessage) {
            //i = sizeof(hexArray) * 8;
            cout << errorMessage;
        }

        // Convert to a hex array

        hexArray = "";


        // Converts to string
        for (int i = 0; i < *(&binaryArray + 1) - binaryArray; i++) {
            hexArray = hexArray.append(to_string(binaryArray[i]));
            // End of the for
        }
        //Converts to hex
        for (int i = 0; i < hexArray.length(); i += 4) {
            if (i + 3 <= hexArray.length()) {
                if (convertString(hexArray, i) == "0000") {
                    newHexArray.append("0");
                }
                else if (convertString(hexArray, i) == "0001") {
                    newHexArray.append("1");
                }
                else if (convertString(hexArray, i) == "0010") {
                    newHexArray.append("2");
                }
                else if (convertString(hexArray, i) == "0011") {
                    newHexArray.append("3");
                }
                else if (convertString(hexArray, i) == "0100") {
                    newHexArray.append("4");
                }
                else if (convertString(hexArray, i) == "0101") {
                    newHexArray.append("5");
                }
                else if (convertString(hexArray, i) == "0110") {
                    newHexArray.append("6");
                }
                else if (convertString(hexArray, i) == "0111") {
                    newHexArray.append("7");
                }
                else if (convertString(hexArray, i) == "1000") {
                    newHexArray.append("8");
                }
                else if (convertString(hexArray, i) == "1001") {
                    newHexArray.append("9");
                }
                else if (convertString(hexArray, i) == "1010") {
                    newHexArray.append("A");
                }
                else if (convertString(hexArray, i) == "1011") {
                    newHexArray.append("B");
                }
                else if (convertString(hexArray, i) == "1100") {
                    newHexArray.append("C");
                }
                else if (convertString(hexArray, i) == "1101") {
                    newHexArray.append("D");
                }
                else if (convertString(hexArray, i) == "1110") {
                    newHexArray.append("E");
                }
                else if (convertString(hexArray, i) == "1111") {
                    newHexArray.append("F");
                }
            }
            // End of the for
        }
        // End of the if
    }
    return newHexArray;
}

int main() {
    string testString("a5391739e67");
    string scrambled;
    scrambled = scrambleHex(testString);
    cout << scrambled;
    return 0;
}