#include "a7105.hpp"
class FHSS{
    private:
    static constexpr uint8_t kChannelCount = 14;
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
    uint8_t bind_pin_; // Arduino pin idx for a Low enable bind button
    volatile uint8_t* bind_in_port_;
    uint8_t bind_bitmask_;

    bool bindButtonPressed(){
        return (*bind_in_port_ & bind_bitmask_) == 0;
    }

    public:

    explicit FHSS(A7105& modem, uint8_t bind_pin) : 
    modem_{modem}, bind_pin_{bind_pin}, is_binding_{false}, bind_countdown_{0},
    tx_id_{0x12345678}, freq_idx_{0}{
        uint8_t port = digitalPinToPort(bind_pin_);
        bind_in_port_ = portInputRegister(port);
        bind_bitmask_ = digitalPinToBitMask(bind_pin_);
        for (int i=0; i<kChannelCount; i++){
            channels_[i] = 1500; // TODO set throttle to 0
            // 0=-125%, 204=-100%
        }
    }
    bool initialize(){
        pinMode(bind_pin_, INPUT_PULLUP);
        return modem_.initialize();
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
        memset(buffer_ + 5, 0xFF, 4);

        buffer_[9] = odd ? 0x01 : 0x00;
        buffer_[10] = 0x00;
        // RF table
        for (uint8_t i = 0; i < 16; i++)
        buffer_[i + 11] = hopping_frequency[odd ? i+16 : i ];
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
            packet[9 + i * 2] = channels_[i] & 0xFF; 
            // high byte of servo timing(1000-2000us)
            packet[10 + i * 2] = (channels_[i] >> 8) & 0xFF; 
        }
        // The last four bytes contain the frequency table index for the next packet
        // Weird format
        buffer_[33] = 0xDC;
        buffer_[34] = (((freq_idx_+1) & 0x0F) << 4) + 0x0F;
        buffer_[35] = 0xDC;
        buffer_[36] = ((freq_idx_+1) & 0x30) + 0x0F;

        modem_.tx(freq_table_[freq_idx_] , buffer_, 37);
        freq_idx_ = (freq_idx_ + 1) & 0x3F; // Take lower 6 bits, equivalent to mod 32
    }

    uint16_t callback(){
        if (bindButtonPressed() && !is_binding_){
            is_binding_ = true;
            bind_countdown_ = 2000; // TODO
            debugln("Binding start");
        }
        if (is_binding_){
            sendBindPacket();
            if (bind_countdown_ == 0){
                is_binding_ = false;
                debugln("Binding complete");
            }
        } else {
            sendNormalPacket();
        }
        return 3852
    }

};