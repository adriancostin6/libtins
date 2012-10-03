#include <gtest/gtest.h>
#include <algorithm>
#include <memory>
#include <stdint.h>
#include "dot11.h"
#include "rsn_information.h"
#include "tests/dot11.h"


using namespace std;
using namespace Tins;

typedef Dot11::address_type address_type;
typedef Dot11Beacon::channels_type channels_type;

class Dot11BeaconTest : public testing::Test {
public:
    static const address_type empty_addr, hwaddr;
    static const uint8_t expected_packet[];
};

const address_type Dot11BeaconTest::empty_addr,
                   Dot11BeaconTest::hwaddr("72:91:34:fa:de:ad");
                   
const uint8_t Dot11BeaconTest::expected_packet[] = { 
    '\x81', '\x01', 'O', '#', '\x00', '\x01', '\x02', '\x03', '\x04', 
    '\x05', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x02', 
    '\x03', '\x04', '\x05', '\x06', '\x07', '\x00', '\x00', '\xfa', 
    '\x01', '\x93', '(', 'A', '#', '\xad', '\x1f', '\xfa', '\x14', 
    '\x95', ' '
};

void test_equals_expected(const Dot11Beacon &dot11) {
    EXPECT_EQ(dot11.subtype(), 8);
    EXPECT_EQ(dot11.timestamp(), 0x1fad2341289301faLL);
    EXPECT_EQ(dot11.interval(), 0x14fa);
    
    const Dot11Beacon::CapabilityInformation &info = dot11.capabilities();
    EXPECT_EQ(info.ess(), 1);
    EXPECT_EQ(info.ibss(), 0);
    EXPECT_EQ(info.cf_poll(), 1);
    EXPECT_EQ(info.cf_poll_req(), 0);
    EXPECT_EQ(info.privacy(), 1);
    EXPECT_EQ(info.short_preamble(), 0);
    EXPECT_EQ(info.pbcc(), 0);
    EXPECT_EQ(info.channel_agility(), 1);
    EXPECT_EQ(info.spectrum_mgmt(), 0);
    EXPECT_EQ(info.qos(), 0);
    EXPECT_EQ(info.sst(), 0);
    EXPECT_EQ(info.apsd(), 0);
    EXPECT_EQ(info.reserved(), 0);
    EXPECT_EQ(info.dsss_ofdm(), 1);
    EXPECT_EQ(info.delayed_block_ack(), 0);
    EXPECT_EQ(info.immediate_block_ack(), 0);
    
    ::test_equals_expected(static_cast<const Dot11ManagementFrame&>(dot11));
}

void test_equals(const Dot11Beacon& b1, const Dot11Beacon& b2) {
    EXPECT_EQ(b1.interval(), b2.interval());
    EXPECT_EQ(b1.timestamp(), b2.timestamp());
    
    test_equals(b1.capabilities(), b2.capabilities());

    test_equals(
        static_cast<const Dot11ManagementFrame&>(b1), 
        static_cast<const Dot11ManagementFrame&>(b2)
    );
}
                   
TEST_F(Dot11BeaconTest, DefaultConstructor) {
    Dot11Beacon dot11;
    test_equals_empty(static_cast<const Dot11ManagementFrame&>(dot11));
    test_equals_empty(dot11.capabilities());
    
    EXPECT_EQ(dot11.interval(), 0);
    EXPECT_EQ(dot11.timestamp(), 0);
    EXPECT_EQ(dot11.subtype(), Dot11::BEACON);
}

// beacon_interval=0x14fa, timestamp=0x1fad2341289301fa, cap="ESS+CFP+privacy+DSSS-OFDM"
TEST_F(Dot11BeaconTest, ConstructorFromBuffer) {
    Dot11Beacon dot11(expected_packet, sizeof(expected_packet));
    test_equals_expected(dot11);
}

TEST_F(Dot11BeaconTest, CopyConstructor) {
    Dot11Beacon dot1(expected_packet, sizeof(expected_packet));
    Dot11Beacon dot2(dot1);
    test_equals(dot1, dot2);
}

TEST_F(Dot11BeaconTest, CopyAssignmentOperator) {
    Dot11Beacon dot1(expected_packet, sizeof(expected_packet));
    Dot11Beacon dot2;
    dot2 = dot1;
    test_equals(dot1, dot2);
}

TEST_F(Dot11BeaconTest, FromBytes) {
    std::auto_ptr<PDU> dot11(Dot11::from_bytes(expected_packet, sizeof(expected_packet)));
    ASSERT_TRUE(dot11.get());
    const Dot11Beacon *beacon = dot11->find_pdu<Dot11Beacon>();
    ASSERT_TRUE(beacon);
    test_equals_expected(*beacon);
}

TEST_F(Dot11BeaconTest, Timestamp) {
    Dot11Beacon dot11;
    dot11.timestamp(0x1fad2341289301faLL);
    EXPECT_EQ(dot11.timestamp(), 0x1fad2341289301faLL);
}

TEST_F(Dot11BeaconTest, Interval) {
    Dot11Beacon dot11;
    dot11.interval(0x14fa);
    EXPECT_EQ(dot11.interval(), 0x14fa);
}

TEST_F(Dot11BeaconTest, SSID) {
    Dot11Beacon dot11;
    dot11.ssid("libtins");
    EXPECT_EQ(dot11.ssid(), "libtins");
}

TEST_F(Dot11BeaconTest, SupportedRates) {
    Dot11Beacon dot11;
    Dot11Beacon::rates_type rates, found_rates;
    rates.push_back(0.5f);
    rates.push_back(1.0f);
    rates.push_back(5.5f);
    rates.push_back(7.5f);
    dot11.supported_rates(rates);
    found_rates = dot11.supported_rates();
    EXPECT_EQ(rates, found_rates);
}

TEST_F(Dot11BeaconTest, ExtendedSupportedRates) {
    Dot11Beacon dot11;
    Dot11Beacon::rates_type rates, found_rates;
    rates.push_back(0.5f);
    rates.push_back(1.0f);
    rates.push_back(5.5f);
    rates.push_back(7.5f);
    dot11.extended_supported_rates(rates);
    found_rates = dot11.extended_supported_rates();
    EXPECT_EQ(rates, found_rates);
}

TEST_F(Dot11BeaconTest, QOSCapability) {
    Dot11Beacon dot11;
    dot11.qos_capability(0xfa);
    EXPECT_EQ(dot11.qos_capability(), 0xfa);
}

TEST_F(Dot11BeaconTest, PowerCapability) {
    typedef std::pair<uint8_t, uint8_t> power_pair;
    
    Dot11Beacon dot11;
    dot11.power_capability(0xfa, 0xa2);
    power_pair power = dot11.power_capability();
    EXPECT_EQ(power.first, 0xfa);
    EXPECT_EQ(power.second, 0xa2);
}

TEST_F(Dot11BeaconTest, SupportedChannels) {
    Dot11Beacon dot11;
    channels_type channels, output;
    channels.push_back(std::make_pair(13, 19));
    channels.push_back(std::make_pair(67, 159));
    dot11.supported_channels(channels);
    output = dot11.supported_channels();
    EXPECT_EQ(output, channels);
}

TEST_F(Dot11BeaconTest, RequestInformation) {
    Dot11Beacon dot11;
    Dot11Beacon::request_info_type info, found_info;
    info.push_back(10);
    info.push_back(15);
    info.push_back(51);
    info.push_back(42);
    dot11.request_information(info);
    found_info = dot11.request_information();
    EXPECT_EQ(info, found_info);
}

TEST_F(Dot11BeaconTest, FHParameterSet) {
    Dot11Beacon dot11;
    Dot11Beacon::fh_params_set params(0x482f, 67, 42, 0xa1), output;
    dot11.fh_parameter_set(params);
    output = dot11.fh_parameter_set();
    EXPECT_EQ(output.hop_index, params.hop_index);
    EXPECT_EQ(output.hop_pattern, params.hop_pattern);
    EXPECT_EQ(output.hop_set, params.hop_set);
    EXPECT_EQ(output.dwell_time, params.dwell_time);
}

TEST_F(Dot11BeaconTest, DSParameterSet) {
    Dot11Beacon dot11;
    dot11.ds_parameter_set(0x1e);
    EXPECT_EQ(dot11.ds_parameter_set(), 0x1e);
}

TEST_F(Dot11BeaconTest, IBSSParameterSet) {
    Dot11Beacon dot11;
    dot11.ibss_parameter_set(0x1ef3);
    EXPECT_EQ(dot11.ibss_parameter_set(), 0x1ef3);
}

TEST_F(Dot11BeaconTest, IBSS_DFS) {
    Dot11Beacon dot11;
    Dot11Beacon::ibss_dfs_params params, output;
    params.dfs_owner = "00:01:02:03:04:05";
    params.recovery_interval = 0x7f;
    params.channel_map.push_back(std::make_pair(0x8e, 0x92));
    params.channel_map.push_back(std::make_pair(0x02, 0xf2));
    params.channel_map.push_back(std::make_pair(0x3a, 0x53));
    dot11.ibss_dfs(params);
    output = dot11.ibss_dfs();
    EXPECT_EQ(params.dfs_owner, output.dfs_owner);
    EXPECT_EQ(params.recovery_interval, output.recovery_interval);
    EXPECT_EQ(params.channel_map, output.channel_map);
}

TEST_F(Dot11BeaconTest, Country) {
    Dot11Beacon dot11;
    Dot11Beacon::country_params params, output;
    params.country = "ARO";
    params.first_channel.push_back(65);
    params.first_channel.push_back(11);
    params.first_channel.push_back(97);
    
    params.number_channels.push_back(123);
    params.number_channels.push_back(56);
    params.number_channels.push_back(42);
    
    params.max_transmit_power.push_back(4);
    params.max_transmit_power.push_back(213);
    params.max_transmit_power.push_back(165);
    
    dot11.country(params);
    output = dot11.country();
    
    EXPECT_EQ(params.country, output.country);
    EXPECT_EQ(params.first_channel, output.first_channel);
    EXPECT_EQ(params.number_channels, output.number_channels);
    EXPECT_EQ(params.max_transmit_power, output.max_transmit_power);
}

TEST_F(Dot11BeaconTest, FHParameters) {
    Dot11Beacon dot11;
    std::pair<uint8_t, uint8_t> tim(0x42, 0x1f);
    dot11.fh_parameters(tim.first, tim.second);
    EXPECT_EQ(tim, dot11.fh_parameters());
}

TEST_F(Dot11BeaconTest, FHPattern) {
    Dot11Beacon dot11;
    Dot11Beacon::fh_pattern_type tim, output;
    tim.flag = 0x67;
    tim.number_of_sets = 0x42;
    tim.modulus = 0x1f;
    tim.offset = 0x3a;
    tim.random_table.push_back(23);
    tim.random_table.push_back(15);
    tim.random_table.push_back(129);
    
    dot11.fh_pattern_table(tim);
    output = dot11.fh_pattern_table();
    
    EXPECT_EQ(tim.flag, tim.flag);
    EXPECT_EQ(tim.number_of_sets, tim.number_of_sets);
    EXPECT_EQ(tim.modulus, tim.modulus);
    EXPECT_EQ(tim.offset, tim.offset);
    EXPECT_EQ(tim.random_table, tim.random_table);
}

TEST_F(Dot11BeaconTest, PowerConstraint) {
    Dot11Beacon dot11;
    dot11.power_constraint(0x1e);
    EXPECT_EQ(dot11.power_constraint(), 0x1e);
}

TEST_F(Dot11BeaconTest, ChannelSwitch) {
    Dot11Beacon dot11;
    Dot11Beacon::channel_switch_type tim(13, 42, 98), output;
    dot11.channel_switch(tim);
    
    output = dot11.channel_switch();
    EXPECT_EQ(output.switch_mode, tim.switch_mode);
    EXPECT_EQ(output.new_channel, tim.new_channel);
    EXPECT_EQ(output.switch_count, tim.switch_count);
}


TEST_F(Dot11BeaconTest, Quiet) {
    Dot11Beacon dot11;
    Dot11Beacon::quiet_type tim(13, 42, 0x928f, 0xf1ad), output;
    dot11.quiet(tim);
    
    output = dot11.quiet();
    EXPECT_EQ(output.quiet_count, tim.quiet_count);
    EXPECT_EQ(output.quiet_period, tim.quiet_period);
    EXPECT_EQ(output.quiet_duration, tim.quiet_duration);
    EXPECT_EQ(output.quiet_offset, tim.quiet_offset);
}

TEST_F(Dot11BeaconTest, TPCReport) {
    Dot11Beacon dot11;
    std::pair<uint8_t, uint8_t> tim(42, 193);
    dot11.tpc_report(tim.first, tim.second);
    EXPECT_EQ(dot11.tpc_report(), tim);
}

TEST_F(Dot11BeaconTest, ERPInformation) {
    Dot11Beacon dot11;
    dot11.erp_information(0x1e);
    EXPECT_EQ(dot11.erp_information(), 0x1e);
}

TEST_F(Dot11BeaconTest, BSSLoad) {
    Dot11Beacon dot11;
    Dot11Beacon::bss_load_type tim(0x129f, 42, 0xf5a2), output;
    dot11.bss_load(tim);
    output = dot11.bss_load();
    
    EXPECT_EQ(tim.station_count, output.station_count);
    EXPECT_EQ(tim.channel_utilization, output.channel_utilization);
    EXPECT_EQ(tim.available_capacity, output.available_capacity);
}

TEST_F(Dot11BeaconTest, TIM) {
    Dot11Beacon dot11;
    Dot11Beacon::tim_type tim, output;
    tim.dtim_count = 42;
    tim.dtim_period = 59;
    tim.bitmap_control = 191;
    
    tim.partial_virtual_bitmap.push_back(92);
    tim.partial_virtual_bitmap.push_back(182);
    tim.partial_virtual_bitmap.push_back(212);
    
    dot11.tim(tim);
    output = dot11.tim();
    
    EXPECT_EQ(tim.dtim_count, output.dtim_count);
    EXPECT_EQ(tim.dtim_period, output.dtim_period);
    EXPECT_EQ(tim.bitmap_control, output.bitmap_control);
    EXPECT_EQ(tim.partial_virtual_bitmap, output.partial_virtual_bitmap);
}

TEST_F(Dot11BeaconTest, ChallengeText) {
    Dot11Beacon dot11;
    dot11.challenge_text("libtins ftw");
    EXPECT_EQ(dot11.challenge_text(), "libtins ftw");
}

TEST_F(Dot11BeaconTest, RSNInformationTest) {
    Dot11Beacon dot11;
    RSNInformation rsn_info, found;
    rsn_info.add_pairwise_cypher(RSNInformation::WEP_40);
    rsn_info.add_akm_cypher(RSNInformation::PSK);
    rsn_info.group_suite(RSNInformation::CCMP);
    rsn_info.version(0x7283);
    rsn_info.capabilities(0x18ad);
    dot11.rsn_information(rsn_info);
    found = dot11.rsn_information();
    
    EXPECT_EQ(rsn_info.version(), found.version());
    EXPECT_EQ(rsn_info.capabilities(), found.capabilities());
    EXPECT_EQ(rsn_info.group_suite(), found.group_suite());
    EXPECT_EQ(rsn_info.pairwise_cyphers(), found.pairwise_cyphers());
    EXPECT_EQ(rsn_info.akm_cyphers(), found.akm_cyphers());
}


TEST_F(Dot11BeaconTest, PCAPLoad1) {
    const uint8_t buffer[] = {
        '\x80', '\x00', '\x00', '\x00', '\xff', '\xff', '\xff', '\xff', 
        '\xff', '\xff', '\xf4', '\xec', '8', '\xfe', 'M', '\x92', '\xf4', 
        '\xec', '8', '\xfe', 'M', '\x92', '\xe0', '\xea', '\x80', '\xd1', 
        '\xd4', '\xce', ',', '\x00', '\x00', '\x00', 'd', '\x00', '1', 
        '\x04', '\x00', '\x07', 'S', 'e', 'g', 'u', 'n', 'd', 'o', '\x01', 
        '\x08', '\x82', '\x84', '\x8b', '\x96', '\x0c', '\x12', '\x18', '$', 
        '\x03', '\x01', '\x01', '\x05', '\x04', '\x00', '\x01', '\x00', 
        '\x00', '\x07', '\x06', 'U', 'S', ' ', '\x01', '\r', '\x14', '*', 
        '\x01', '\x00', '0', '\x14', '\x01', '\x00', '\x00', '\x0f', '\xac', 
        '\x04', '\x01', '\x00', '\x00', '\x0f', '\xac', '\x04', '\x01', 
        '\x00', '\x00', '\x0f', '\xac', '\x02', '\x00', '\x00', '2', '\x04', 
        '0', 'H', '`', 'l', '\xdd', '\x18', '\x00', 'P', '\xf2', '\x02', 
        '\x01', '\x01', '\x03', '\x00', '\x03', '\xa4', '\x00', '\x00', 
        '\'', '\xa4', '\x00', '\x00', 'B', 'C', '^', '\x00', 'b', '2', 
        '/', '\x00', '\xdd', '\t', '\x00', '\x03', '\x7f', '\x01', '\x01', 
        '\x00', '\x00', '\xff', '\x7f'
    };
    typedef Dot11Beacon::country_params::container_type country_container;
    Dot11Beacon dot11(buffer, sizeof(buffer));
    float rates[] = { 1.0f, 2.0f, 5.5f, 11.0f, 6.0f, 9.0f, 12.0f, 18.0f},
           ext_rates[] = { 24.0f, 36.0f, 48.0f, 54.0f };
    Dot11Beacon::rates_type rates_parsed = dot11.supported_rates();
    Dot11Beacon::rates_type ext_rates_parsed = dot11.extended_supported_rates();
    Dot11Beacon::tim_type tim(0, 1, 0, Dot11Beacon::tim_type::container_type(1)), 
                             tim_parsed = dot11.tim();
    Dot11Beacon::country_params country("US ", 
                                            country_container(1, 1),
                                            country_container(1, 13),
                                            country_container(1, 20)),
                                    country_parsed = dot11.country();
    EXPECT_EQ(dot11.ssid(), "Segundo");
    ASSERT_EQ(rates_parsed.size(), sizeof(rates) / sizeof(float));
    EXPECT_TRUE(std::equal(rates_parsed.begin(), rates_parsed.end(), rates));
    ASSERT_EQ(ext_rates_parsed.size(), sizeof(ext_rates) / sizeof(float));
    EXPECT_TRUE(std::equal(ext_rates_parsed.begin(), ext_rates_parsed.end(), ext_rates));
    EXPECT_EQ(1, dot11.ds_parameter_set());
    EXPECT_EQ(tim.dtim_count, tim_parsed.dtim_count);
    EXPECT_EQ(tim.dtim_period, tim_parsed.dtim_period);
    EXPECT_EQ(tim.bitmap_control, tim_parsed.bitmap_control);
    EXPECT_EQ(tim.partial_virtual_bitmap, tim_parsed.partial_virtual_bitmap);
    EXPECT_EQ(country.country, country_parsed.country);
    EXPECT_EQ(country.first_channel, country_parsed.first_channel);
    EXPECT_EQ(country.number_channels, country_parsed.number_channels);
    EXPECT_EQ(country.max_transmit_power, country_parsed.max_transmit_power);
    EXPECT_EQ(dot11.erp_information(), 0);
    
    PDU::serialization_type serialized = dot11.serialize();
    ASSERT_EQ(sizeof(buffer), serialized.size());
    EXPECT_TRUE(std::equal(serialized.begin(), serialized.end(), buffer));
}

TEST_F(Dot11BeaconTest, Serialize) {
    Dot11Beacon pdu(expected_packet, sizeof(expected_packet));
    PDU::serialization_type buffer = pdu.serialize();
    ASSERT_EQ(sizeof(expected_packet), buffer.size());
    EXPECT_TRUE(std::equal(buffer.begin(), buffer.end(), expected_packet));
}