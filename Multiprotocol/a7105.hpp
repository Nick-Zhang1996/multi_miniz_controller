#include <Arduino.h>
#include "SPI.h"
#include "Multiprotocol.h"
class A7105
{
    private:
    volatile uint8_t *cs_port_;
    uint8_t cs_bit_mask_;

    public:

    // Strobe Commands mapped directly from the A7105 Datasheet
    enum StrobeCommand : uint8_t
    {
        kSleepMode = 0b10000000,
        kIdleMode = 0b10010000,
        kStandbyMode = 0b10100000,
        kPllMode = 0b10110000,
        kRxMode = 0b11000000,
        kTxMode = 0b11010000,
        kFifoWriteReset = 0b11100000,
        kFifoReadReset = 0b11110000
    };

    A7105(const A7105&) = delete;
    explicit A7105(uint8_t cs_pin)
    {
        uint8_t port = digitalPinToPort(cs_pin);
        cs_port_ = portOutputRegister(port);
        cs_bit_mask_ = digitalPinToBitMask(cs_pin);
    }
    inline void csEnable()
    {
        *cs_port_ &= ~_BV(cs_bit_mask_); // CSN: Low enable
    }
    inline void csDisable()
    {
        *cs_port_ |= _BV(cs_bit_mask_); // CSN: High disable
    }

    void writeReg(uint8_t address, uint8_t data)
    {
        csEnable();
        SPI_Write(address); // Software SPI defined by Multiprotocol lib
        _NOP();
        SPI_Write(data);
        csDisable();
        delay1us();
    }

    uint8_t readReg(uint8_t address)
    {
        csEnable();
        SPI_Write(address |= 0x40); // bit 6 = 1 means reading for A7105
        _NOP();
        uint8_t result = SPI_SDI_Read();
        csDisable();
        return result;
    }

    // Write to ID Data reg, ID hard-coded to Kyosho ID
    void writeID()
    {
        csEnable();
        SPI_Write(0x06); // ID register
        SPI_Write(0x54); // Kyosho's ID for KT531p
        SPI_Write(0x75);
        SPI_Write(0xC5);
        SPI_Write(0x2A);
        csDisable();
    }

    void strobe(StrobeCommand cmd)
    {
        csEnable();
        SPI_Write(cmd);
        csDisable();
        delay1us();
    }

    // Transmit to air
    void tx(uint8_t channel, uint8_t *buffer, uint8_t len)
    {
        writeReg(0x0F, channel);
        csEnable();
        for (int i = 0; i < len; ++i)
        {
            SPI_Write(buffer[i]);
        }
        csDisable();
        strobe(kTxMode);
    }
    void delay1us()
    {
        for (int i = 0; i < 16; i++)
        {
            _NOP(); // 62.5
        }
    }

    // Replay captured setup sequence from KT531p,
    // for details refer to capture.csv and datasheet
    bool initialize()
    {
        SDI_output;
        SCLK_output;
        A7105_CSN_output;

        BIND_SET_INPUT;
        BIND_SET_PULLUP;
        A7105_CSN_on;
        SDI_on;
        SCLK_off;

        bool init_success = true;
        // Mode register, write to reset, will auto clear
        writeReg(0x00, 0x00);
        delay1us();
        delay1us();
        delay1us();
        delay1us();
        delay1us();
        delay1us();
        delay1us();
        // set ID to Kyosho ID
        writeID();
        // Mode Control reg, Enable Auto RSSI, FIFO mode
        writeReg(0x01, 0x42);
        // FIFO Reg 1, End Pointer for TX/RX , TODO ch 16 for detail
        writeReg(0x03, 0x25);
        // FIFO Reg 2
        writeReg(0x04, 0x00);
        // RC osc Register I, internal, write to reset
        writeReg(0x07, 0x00);
        // II, ditto
        writeReg(0x08, 0x00);
        // III, ditto
        writeReg(0x09, 0x00);
        //
        writeReg(0x0A, 0x00);
        // Reset GIO1 PIN control reg
        writeReg(0x0B, 0x01);
        // GIO2 Pin Control, RX Direct mode, enable
        writeReg(0x0C, 0x21);
        // CLock register, set internal divider = /2
        writeReg(0x0D, 0x05);
        //
        writeReg(0x0E, 0x00);
        // PLL register, LO channel number select TODO ch14
        writeReg(0x0F, 0x50);
        // PLL reg II, TODO ch14
        writeReg(0x10, 0x9E);
        //
        writeReg(0x11, 0x4B);
        //
        writeReg(0x12, 0x00);
        // LO base freq fractional part
        writeReg(0x13, 0x02);
        // TX reg
        writeReg(0x14, 0x16);
        //
        writeReg(0x15, 0x2B);
        //
        writeReg(0x16, 0x12);
        //
        writeReg(0x17, 0x40);
        // Below are RX registers, some are marked internal use, maybe related to determine failsafe based on RSSI value
        writeReg(0x18, 0x62);
        writeReg(0x19, 0x80);
        writeReg(0x1A, 0x80);
        writeReg(0x1B, 0x00);
        writeReg(0x1C, 0x0A);
        writeReg(0x1D, 0x32);
        writeReg(0x1E, 0x03);
        writeReg(0x1F, 0x1F);
        writeReg(0x20, 0x1E);
        writeReg(0x21, 0x00);
        // IF Calibfration Register,
        writeReg(0x22, 0x00);
        // Other calibrationr registers
        writeReg(0x24, 0x00);
        writeReg(0x25, 0x00);
        writeReg(0x26, 0x23);
        writeReg(0x27, 0x70);
        writeReg(0x28, 0x1F);
        writeReg(0x29, 0x47);
        writeReg(0x2A, 0x80);
        writeReg(0x2B, 0x57);
        writeReg(0x2C, 0x01);
        writeReg(0x2D, 0x45);
        writeReg(0x2E, 0x19);
        writeReg(0x2F, 0x00);
        writeReg(0x30, 0x01);
        writeReg(0x31, 0x0F);
        writeReg(0x33, 0x7F);
        strobe(kStandbyMode);

        // Calibration Control Register, enable all, autoclear when done.
        // Though bit 3 is undefined, we set it as Kyosho did
        writeReg(0x02, 0x0F);

        // Keep reading 0x02, wait till done
        uint8_t retval;
        for (int i = 0; i < 100; i++)
        {
            // Wait for Autoclear
            retval = readReg(0x42);
            if (retval == 0)
            {
                break;
            }
            delay1us();
            delay1us();
        }
        if (retval)
        {
            debugln("0x42 incomplete after 100 cycles");
            init_success = false;
        }
        delay1us();
        // Read 0x22, IF calibration  bit4 == 0 -> pass
        retval = readReg(0x62);
        if (retval & (1 << 4))
        {
            debugln("IF calibration fail");
            init_success = false;
        }
        delay1us();
        // VCH calibration, bit4 == 0 -> pass
        retval = readReg(0x64);
        if (retval & (1 << 4))
        {
            debugln("VCH calibration fail");
            init_success = false;
        }
        delay1us();
        // VCO calibration bit3 == 0 -> pass
        retval = readReg(0x65);
        if (retval & (1 << 3))
        {
            debugln("VCO calibration fail");
            init_success = false;
        }
        delay1us();
        // RX Gain reg, internal use
        // sent: 0x5A got: 0x1C
        // RX Gain III, internal use, maybe if there's RX, then transmitter knows there's a car and won't waste time binding, this capture is when no car is around
        // sent: 0x5B  got: 0x42
        strobe(kStandbyMode);
        delayMicroseconds(20); // Matching observed delay, may not be necessary
        strobe(kFifoWriteReset);
        delayMicroseconds(4);
        return init_success;
    }
};