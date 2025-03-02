#include <stdint.h>
#include <stdio.h>

// Definiera CRC-polynomet och CRC-bredden (15-bitars CRC)
#define CRC_WIDTH 0x7FFF // CRC width for 15-bit
#define POLYNOMIAL 0xC599

// Funktion för att skriva ut varje byte i meddelandet som en binärsträng
void printMessageBinary(uint8_t message[])
{
    int i = 0;

    // Loopa igenom varje byte i meddelandet tills vi når nullbyte (0)
    while (message[i] != 0)
    {
        for (int j = 7; j >= 0; j--)
        {
            // Utskrift av varje bit genom att skifta och maskera bitarna
            printf("%d", (message[i] >> j) & 1);
        }
        printf(" "); 
        i++;
    }
    printf("\n");
}

// Funktion för att vända på bitarna i en byte
uint8_t reverse_bits(uint8_t byte)
{
    // Loopa genom varje bit i byten och vänd på bitarna
    uint8_t reversed = 0;
    for (int i = 0; i < 8; i++)
    {
        // Ta den i:te biten från originalbyte och sätt den i rätt position i den omvända byten
        reversed |= ((byte >> i) & 1) << (7 - i); 
    }
    return reversed;
}

// Funktion för att beräkna CRC för ett meddelande
uint16_t calculate_crc(uint8_t *message, uint16_t length)
{
    // Remainder som används för att beräkna CRC
    uint16_t remainder = 0;

    // Loopa igenom varje byte i meddelandet
    for (int i = 0; i < length; i++)
    {
        // Bearbeta varje bit i den aktuella byten från LSB till MSB
        for (int j = 0; j < 8; j++)
        {
            // Skift resten till vänster med 1 bit för att bearbeta nästa bit
            remainder <<= 1;

            // Lägg till den aktuella biten från meddelandet i CRC-resten
            remainder |= (((message[i] >> j) & 1));

            // Om den högsta biten i resten är 1, XOR:a med polynomet
            if (remainder & 0x8000) 
            {
                // Utför XOR med polynomet
                remainder ^= POLYNOMIAL; 
            }
        }
    }

    // Returnera CRC med korrekt bredd (15 bitar)
    return remainder & CRC_WIDTH;
}

int main()
{
    // Meddelandet att beräkna CRC för
    uint8_t message[] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', 0, 0};
    // Längden på meddelandet, beräknad som antal bytes
    uint16_t length = sizeof(message);

    // Kontrollera om längden på meddelandet är mellan 1 och 14 bytes
    if (length < 1 || length > 14)
    {
        printf("The message length is invalid. The length must be between 1 and 14 bytes.\n");
        return 1; // Avsluta programmet om längden inte är korrekt
    }

    // Skriv ut meddelandet som binärsträng
    printMessageBinary(message);

    // Beräkna CRC för meddelandet
    uint16_t crc = calculate_crc(message, length);
    // Skriv ut CRC i hex-format
    printf("Calculated crc: 0X%X \n", crc);

    // Dela upp CRC i höga och låga byte
    uint8_t high_byte = (crc >> 8) & 0xFF;
    uint8_t low_byte = crc & 0xFF;

    // Vänd på bitarna för båda bytena (högt och lågt byte)
    uint8_t reversed_highbyte = reverse_bits(high_byte);
    uint8_t reversed_lowbyte = reverse_bits(low_byte);

    // Sätt de omvända bytena i meddelandet för CRC
    message[length - 2] = reversed_highbyte;
    message[length - 1] = reversed_lowbyte;

    // Skriv ut det nya meddelandet med de omvända CRC-bytena
    printMessageBinary(message);

    // Verifiera CRC genom att beräkna den igen
    uint16_t verify = calculate_crc(message, length);
    // Skriv ut den verifierade CRC:n
    printf("Verification CRC: 0x%X\n", verify);
    printf("\n");

    // Om CRC är 0, så är meddelandet korrekt (inget fel)
    if (verify == 0)
    {
        printf("CRC is valid!\n");
    }
    else
    {
        printf("CRC is corrupted!\n");
    }

    return 0;
}
