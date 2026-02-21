#pragma once
#include "a7105.hpp"
class FHSS{
    public:
    static constexpr uint8_t kChannelCount = 12;
    static constexpr uint16_t kCallbackInterval = 3853; // in us
    uint8_t bind_pin_; // Arduino pin idx for a Low enable bind button

    private:
    A7105& modem_;

    bool is_binding_;
    int bind_countdown_;
    uint8_t buffer_[64];
    uint32_t tx_id_;

    // Frequency hopping table for FHSS
    uint8_t freq_table_[32];
    // Current freq
    uint8_t freq_idx_;

    // RC channel value
    uint16_t channels_[kChannelCount];
    volatile uint8_t* bind_in_port_;
    uint8_t bind_bitmask_;

    bool bindButtonPressed(){
        return (*bind_in_port_ & bind_bitmask_) == 0;
    }

    public:

    // TODO set tx_id, 7CB838, 3D743B
    explicit FHSS(A7105& modem, uint8_t bind_pin, uint32_t tx_id) : 
    modem_{modem}, bind_pin_{bind_pin}, is_binding_{false}, bind_countdown_{0},
    tx_id_{tx_id}, freq_idx_{0}{
        uint8_t port = digitalPinToPort(bind_pin_);
        bind_in_port_ = portInputRegister(port);
        bind_bitmask_ = digitalPinToBitMask(bind_pin_);
        for (int i=0; i<kChannelCount; i++){
            channels_[i] = 1500; // TODO set throttle to 0
            // 0=-125%, 204=-100%
        }

        calc_fh_channels();
    }

    bool initialize(){
        pinMode(bind_pin_, INPUT_PULLUP);
        // Multiple targets may share a modem, so init modem separately
        // modem_.initialize();
        return true;
    }

    void sendBindPacket(){
        // Alternate between two bind packets
        bool odd = bind_countdown_ & 0x01;
        // Bind indicator
        buffer_[0] = 0xBC; 
        // Tx id, different for each transmitter
        buffer_[1] = (tx_id_ >> 16) & 0xFF;
        buffer_[2] = (tx_id_ >> 8) & 0xFF;
        buffer_[3] = tx_id_ & 0xFF;
        buffer_[4] = 0x00;
        // Multiprotocol original, maybe for other protocols
        //buffer_[4] = (buffer_[2] & 0xF0) | (buffer_[3] & 0x0F);
        memset(buffer_ + 5, 0xFF, 4);

        buffer_[9] = odd ? 0x01 : 0x00;
        buffer_[10] = 0x00;
        // RF table
        for (uint8_t i = 0; i < 16; i++)
        buffer_[i + 11] = freq_table_[odd ? i+16 : i ];
        // TX type
        buffer_[27] = 0x05;
        // Unknown
        buffer_[28] = 0x00;
        memset(buffer_ + 29, 0xFF, 8);
        // frequency hop during bind
        modem_.tx( odd ? 0x8C : 0x0D, buffer_, 37);
        bind_countdown_--;
    }
    void sendNormalPacket(){
        // Normal packet indicator
        buffer_[0] = 0x58; 
        // Tx id, different for each transmitter
        buffer_[1] = (tx_id_ >> 16) & 0xFF;
        buffer_[2] = (tx_id_ >> 8) & 0xFF;
        buffer_[3] = tx_id_ & 0xFF;
        buffer_[4] = 0x00;
        memset(buffer_ + 5, 0xFF, 4);
        // FHSS  14 channels: steering, throttle, ...
        for (uint8_t i = 0; i < 12; i++) {
            // low byte of servo timing(1000-2000us)
            buffer_[9 + i * 2] = channels_[i] & 0xFF; 
            // high byte of servo timing(1000-2000us)
            buffer_[10 + i * 2] = (channels_[i] >> 8) & 0xFF; 
        }
        // The last four bytes contain the frequency table index for the next packet
        // Weird format
        buffer_[33] = 0xDC;
        buffer_[34] = (((freq_idx_+1) & 0x0F) << 4) + 0x0F;
        buffer_[35] = 0xDC;
        buffer_[36] = ((freq_idx_+1) & 0x30) + 0x0F;

        modem_.tx(freq_table_[freq_idx_] , buffer_, 37);
    }

    void callback(){
        if (bindButtonPressed() && !is_binding_){
            is_binding_ = true;
            bind_countdown_ = 500;
            debugln("Binding start pin:%d", bind_pin_);
        }
        if (is_binding_){
            sendBindPacket();
            if (bind_countdown_ == 0){
                is_binding_ = false;
                debugln("Binding complete");
            }
        } else {
            // Skip every other packet. 
            // In KT531p implementation, all packets are prepared and loaded to the RF chip
            // But the TX Strobe command is issued only on every other packet.
            // The reason for this is unknown. 
            // However, this means we can safety skip some packets. 
            // This allows us to send to two receivers with one modem
            // Since it takes 3.1 ms to transmit a packet, and there's only 3.8ms between hopping.
            // If we skip half the packets, we have 7.6ms and can alternate between two receivers.
            if (! (freq_idx_ & 0b1)){
                sendNormalPacket();
            }
            freq_idx_ = (freq_idx_ + 1) & 0x3F; // Take lower 6 bits, equivalent to mod 32
        }
    }

    // Generate frequency hopping sequence in the range [02..77]
    // Store at freq_table_
    void  calc_fh_channels() {
        const uint8_t num_ch = 32;
        uint8_t idx = 0;
        uint32_t rnd = tx_id_;
        uint8_t max = (num_ch / 3) + 2;

        while (idx < num_ch) {
            uint8_t i;
            uint8_t count_2_26 = 0, count_27_50 = 0, count_51_74 = 0;

            rnd = rnd * 0x0019660D + 0x3C6EF35F; // Randomization
            // Use least-significant byte. 73 is prime, so channels 76..77 are unused
            uint8_t next_ch = ((rnd >> 8) % 73) + 2;
            // Keep a distance of 5 between consecutive channels
            if (idx != 0) {
            if (freq_table_[idx - 1] > next_ch) {
                if (freq_table_[idx - 1] - next_ch < 5)
                continue;
            } else if (next_ch - freq_table_[idx - 1] < 5)
                continue;
            }
            // Check that it's not duplicated and spread uniformly
            for (i = 0; i < idx; i++) {
            if (freq_table_[i] == next_ch)
                break;
            if (freq_table_[i] <= 26)
                count_2_26++;
            else if (freq_table_[i] <= 50)
                count_27_50++;
            else
                count_51_74++;
            }
            if (i != idx)
            continue;
            if ((next_ch <= 26 && count_2_26 < max) ||
                (next_ch >= 27 && next_ch <= 50 && count_27_50 < max) ||
                (next_ch >= 51 && count_51_74 < max))
            freq_table_[idx++] = next_ch; // find hopping frequency
        }
    }

};