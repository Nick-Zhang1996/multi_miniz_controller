/*
 This project is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Multiprotocol is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Multiprotocol.  If not, see <http://www.gnu.org/licenses/>.
 */

const char STR_FLYSKY[]		="FlySky";
const char STR_HUBSAN[]		="Hubsan";
const char STR_FRSKYD[]		="FrSky D";
const char STR_HISKY[]		="Hisky";
const char STR_V2X2[]		="V2x2";
const char STR_DSM[]		="DSM";
const char STR_DSM_RX[]		="DSM_RX";
const char STR_DEVO[]		="Devo";
const char STR_YD717[]		="YD717";
const char STR_KN[]			="KN";
const char STR_SYMAX[]		="SymaX";
const char STR_SLT[]		="SLT";
const char STR_CX10[]		="CX10";
const char STR_CG023[]		="CG023";
const char STR_BAYANG[]		="Bayang";
const char STR_FRSKYL[]		="FrSky L";
const char STR_FRSKYX[]		="FrSky X";
const char STR_FRSKYX2[]	="FrSkyX2";
const char STR_ESKY[]		="ESky";
const char STR_MT99XX[]		="MT99XX";
const char STR_MT99XX2[]	="MT99XX2";
const char STR_MJXQ[]		="MJXq";
const char STR_SHENQI[]		="Shenqi";
const char STR_SHENQI2[]	="Shenqi2";
const char STR_FY326[]		="FY326";
const char STR_FUTABA[]		="Futaba";
const char STR_J6PRO[]		="J6 Pro";
const char STR_JJRC345[]	="JJRC345";
const char STR_JOYSWAY[]	="JOYSWAY";
const char STR_FQ777[]		="FQ777";
const char STR_ASSAN[]		="Assan";
const char STR_FRSKYV[]		="FrSky V";
const char STR_HONTAI[]		="Hontai";
const char STR_AFHDS2A[]	="FlSky2A";
const char STR_Q2X2[]		="Q2x2";
const char STR_WK2x01[]		="Walkera";
const char STR_Q303[]		="Q303";
const char STR_Q90C[]		="Q90C";
const char STR_GW008[]		="GW008";
const char STR_DM002[]		="DM002";
const char STR_CABELL[]		="Cabell";
const char STR_ESKY150[]	="Esky150";
const char STR_ESKY150V2[]	="EskyV2";
const char STR_H36[]		="H36";
const char STR_H8_3D[]		="H8 3D";
const char STR_CORONA[]		="Corona";
const char STR_CFLIE[]		="CFlie";
const char STR_HITEC[]		="Hitec";
const char STR_WFLY[]		="WFLY";
const char STR_WFLY2[]		="WFLY2";
const char STR_BUGS[]		="Bugs";
const char STR_BUGSMINI[]	="BugMini";
const char STR_TRAXXAS[]	="Traxxas";
const char STR_NCC1701[]	="NCC1701";
const char STR_E01X[]		="E01X";
const char STR_V911S[]		="V911S";
const char STR_GD00X[]		="GD00x";
const char STR_V761[]		="V761";
const char STR_KF606[]		="KF606";
const char STR_REDPINE[]	="Redpine";
const char STR_POTENSIC[]	="Potensi";
const char STR_ZSX[]		="ZSX";
const char STR_HEIGHT[]		="Height";
const char STR_SCANNER[]	="Scanner";
const char STR_FRSKY_RX[]	="FrSkyRX";
const char STR_AFHDS2A_RX[]	="FS2A_RX";
const char STR_HOTT[]		="HoTT";
const char STR_FX[]			="FX";
const char STR_BAYANG_RX[]	="BayanRX";
const char STR_PELIKAN[]	="Pelikan";
const char STR_XK[]			="XK";
const char STR_XK2[]		="XK2";
const char STR_XN297DUMP[]	="XN297DP";
const char STR_FRSKYR9[]	="FrSkyR9";
const char STR_PROPEL[]		="Propel";
const char STR_SKYARTEC[]	="Skyartc";
const char STR_KYOSHO[]		="Kyosho";
const char STR_KYOSHO2[]	="Kyosho2";
const char STR_RLINK[]		="RadLink";
const char STR_REALACC[]	="Realacc";
const char STR_OMP[]		="OMP";
const char STR_MLINK[]		="M-Link";
const char STR_TEST[]		="Test";
const char STR_NANORF[]		="NanoRF";
const char STR_E016HV2[]    ="E016Hv2";
const char STR_E010R5[]     ="E010r5";
const char STR_LOLI[]       ="LOLI";
const char STR_E129[]       ="E129";
const char STR_E016H[]      ="E016H";
const char STR_IKEAANSLUTA[]="Ansluta";
const char STR_CONFIG[]     ="Config";
const char STR_LOSI[]       ="Losi";
const char STR_MOULDKG[]    ="MouldKg";
const char STR_XERALL[]     ="Xerall";
const char STR_SCORPIO[]    ="Scorpio";
const char STR_BLUEFLY[]    ="BlueFly";
const char STR_BUMBLEB[]    ="BumbleB";
const char STR_SGF22[]      ="SGF22";
const char STR_EAZYRC[]     ="EazyRC";
const char STR_KYOSHO3[]    ="Kyosho3";
const char STR_YUXIANG[]    ="YuXiang";
const char STR_UDIRC[]      ="UDIRC";
const char STR_JIABAILE[]   ="JIABAILE";
const char STR_KAMTOM[]     ="KAMTOM";
const char STR_WL91X[]      ="WL91x";
const char STR_WPL[]        ="WPL";

const char STR_SUBTYPE_FLYSKY[] =     "\x04""Std\0""V9x9""V6x6""V912""CX20";
const char STR_SUBTYPE_HUBSAN[] =     "\x04""H107""H301""H501";
const char STR_SUBTYPE_FRSKYD[] =     "\x06""D8\0   ""Cloned";
#ifndef MULTI_EU
	const char STR_SUBTYPE_FRSKYX[] =     "\x07""D16\0   ""D16 8ch""LBT(EU)""LBT 8ch""Cloned\0""Clo 8ch";
#else
	const char STR_SUBTYPE_FRSKYX[] =     "\x07""--->\0  ""--->\0  ""LBT(EU)""LBT 8ch""Cloned\0""Clo 8ch";
#endif
const char STR_SUBTYPE_HISKY[] =      "\x05""Std\0 ""HK310";
const char STR_SUBTYPE_V2X2[] =       "\x06""Std\0  ""JXD506""MR101\0";
#ifndef MULTI_EU
	const char STR_SUBTYPE_DSM[] =        "\x04""2 1F""2 2F""X 1F""X 2F""Auto""R 1F""2SFC";
#else
	const char STR_SUBTYPE_DSM[] =        "\x04""--->""--->""X 1F""X 2F""Auto""R 1F""----";
#endif
const char STR_SUBTYPE_KYOSHO[] =     "\x04""FHSS""Hype";
#define NO_SUBTYPE		nullptr

#ifdef SEND_CPPM
	const char STR_SUB_FRSKY_RX[] =   "\x07""Multi\0 ""CloneTX""EraseTX""CPPM\0  ";
	const char STR_SUB_DSM_RX[] =   "\x07""Multi\0 ""CloneTX""EraseTX""CPPM\0  ";
	#define FRCPPM   4
	#define DSMCPPM   4
	const char STR_CPPM[] =           "\x05""Multi""CPPM\0";
	#define NBR_CPPM 2
#else
	const char STR_SUB_FRSKY_RX[] =   "\x07""Multi\0 ""CloneTX""EraseTX";
	const char STR_SUB_DSM_RX[] =   "\x07""Multi\0 ""CloneTX""EraseTX";
	#define FRCPPM   3
	#define DSMCPPM   3
	#define STR_CPPM NO_SUBTYPE
	#define NBR_CPPM 0
#endif

enum
{
	OPTION_NONE,
	OPTION_OPTION,
	OPTION_RFTUNE,
	OPTION_VIDFREQ,
	OPTION_FIXEDID,
	OPTION_TELEM,
	OPTION_SRVFREQ,
	OPTION_MAXTHR,
	OPTION_RFCHAN,
	OPTION_RFPOWER,
	OPTION_WBUS,
};

const mm_protocol_definition multi_protocols[] = {
// Protocol number, Protocol String, Sub_protocol strings, Number of sub_protocols, Option type, Failsafe, ChMap, RF switch, Init, Callback
	#if defined(KYOSHO_A7105_INO)
		{PROTO_KYOSHO,     STR_KYOSHO,    STR_SUBTYPE_KYOSHO,    2, OPTION_NONE,    0, 1, 0,  KYOSHO_init,     KYOSHO_callback     },
	#endif
		{0xFF,             nullptr,       nullptr,               0, 0,              0, 0, 0,         nullptr,         nullptr             }
};

#ifdef MULTI_TELEMETRY
uint16_t PROTOLIST_callback()
{
	if(option != prev_option)
	{//Only send once
		/* Type 0x11 Protocol list export via telemetry. Used by the protocol PROTO_PROTOLIST=0, the list entry is given by the Option field.
		   length: variable
		   data[0]     = protocol number, 0xFF is an invalid list entry (Option value too large), Option == 0xFF -> number of protocols in the list
		   data[1..n]  = protocol name null terminated
		   data[n+1]   = flags
						 flags>>4 Option text number to be displayed (check multi status for description)
						 flags&0x01 failsafe supported
						 flags&0x02 Channel Map Disabled supported
		   data[n+2]   = number of sub protocols
		   data[n+3]   = sub protocols text length, only sent if nbr_sub != 0
		   data[n+4..] = sub protocol names, only sent if nbr_sub != 0
		*/
		prev_option = option;

		if(option >= (sizeof(multi_protocols)/sizeof(mm_protocol_definition)) - 1)
		{//option is above the end of the list
			//Header
			multi_send_header(MULTI_TELEMETRY_PROTO, 1);
			if(option == 0xFF)
				Serial_write((sizeof(multi_protocols)/sizeof(mm_protocol_definition)) - 1);	//Nbr proto
			else
				Serial_write(0xFF);															//Error
		}
		else
		{//valid option value
			uint8_t proto_len = strlen(multi_protocols[option].ProtoString) + 1;
			uint8_t nbr_sub = multi_protocols[option].nbrSubProto;
			uint8_t sub_len = 0;
			if(nbr_sub)
				sub_len = multi_protocols[option].SubProtoString[0];
			
			//Header
			multi_send_header(MULTI_TELEMETRY_PROTO, 1 + proto_len + 1 + 1 + (nbr_sub?1:0) + (nbr_sub * sub_len));
			//Protocol number
			Serial_write(multi_protocols[option].protocol);
			//Protocol name
			for(uint8_t i=0;i<proto_len;i++)
			{
				Serial_write(multi_protocols[option].ProtoString[i]);
				//debug("%c",multi_protocols[option].ProtoString[i]);
			}
			//Flags
			uint8_t flags=0;
			#ifdef FAILSAFE_ENABLE
				if(multi_protocols[option].failSafe)
					flags |= 0x01;		//Failsafe supported
			#endif
			if(multi_protocols[option].chMap)
				flags |= 0x02;			//Disable_ch_mapping supported
			Serial_write( flags | (multi_protocols[option].optionType<<4));	// flags && option type
			//debug(" Flag=%02X",flags | (multi_protocols[option].optionType<<4));
			//Number of sub protocols
			Serial_write(nbr_sub);
			//debug(" NSub=%02X ",nbr_sub);
			if(nbr_sub !=0 )
			{//Sub protocols length and texts
				for(uint8_t i=0;i<=nbr_sub*sub_len;i++)
				{
					Serial_write(multi_protocols[option].SubProtoString[i]);
					//debug("%c",multi_protocols[option].SubProtoString[i]);
				}
			}
			//debugln("");
		}
	}
	return 1000;
}
#endif
