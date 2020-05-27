#ifndef WOWSERVER_WORLDPACKET_H
#define WOWSERVER_WORLDPACKET_H

#include "common/defines.h"
#include "common/ByteBuffer.h"


#define PACKET_HEADER_LEN 6
#pragma pack(push,1)
struct SERVER_DECL PacketHeader
{
	uint32 size;
	uint16 cmd;
	//uint8 buf[0];
};
#pragma pack(pop)


class SERVER_DECL WorldPacket : public ByteBuffer
{
public:
	GE_INLINE void WriteBool(bool n) { *this << n; }
	GE_INLINE void WriteChar(char n) { *this << n; }
	GE_INLINE void WriteString(std::string n) { *this << n; }
	GE_INLINE void WriteInt8(int8 n) { *this << n; }
	GE_INLINE void WriteUint8(uint8 n) { *this << n; }
	GE_INLINE void WriteInt16(int16 n) { *this << n; }
	GE_INLINE void WriteInt16(int16 n, int pos )
	{
		int16* tempN = &n;
		const uint8* tempUint8 = (const uint8*)tempN;
		int size = sizeof( int16 );
		put( pos, tempUint8, size );
	}
	GE_INLINE void WriteUint16(uint16 n) { *this << n; }
	GE_INLINE void WriteInt32(int32 n) { *this << n; }
	GE_INLINE void WriteInt32(int n, int pos )
	{
		int* tempN = &n;
		const uint8* tempUint8 = (const uint8*)tempN;
		int size = sizeof( int );
		put( pos, tempUint8, size );
	}

	GE_INLINE void WriteUint32(uint32 n) { *this << n; }
	GE_INLINE void WriteInt64(int64 n) { *this << n; }
	GE_INLINE void WriteUint64(uint64 n) { *this << n; }
	GE_INLINE void WriteFloat(float n) { *this << n; }
	GE_INLINE void WriteDouble(double n) { *this << n; }
	void WriteByte( const char* dest, size_t len )
	{
		this->append( dest, len );
	}
	void WriteContent( const string& str )
	{
		const char* strChar = str.c_str();
//		const int* opCode = strChar;
//		int opcode = *opCode;
		int opcode = *(int*)(strChar);
		this->SetOpcode( opcode );
		this->WriteByte( strChar, str.length() );
	}
//	void WriteByte( )

	GE_INLINE bool ReadBool() { return read<bool>(); }
	GE_INLINE char ReadChar() { return read<char>(); }
	GE_INLINE void ReadStringFast(std::string& str) { *this >> str; }
	GE_INLINE std::string ReadString() { std::string str;*this >> str;return str; }
	GE_INLINE int8 ReadInt8() { int8 r; *this >> r; return r; }
	GE_INLINE uint8 ReadUint8() { uint8 r; *this >> r; return r; }
	GE_INLINE int16 ReadInt16() { int16 r; *this >> r; return r; }
	GE_INLINE uint16 ReadUint16() { uint16 r; *this >> r; return r; }
	GE_INLINE int32 ReadInt32() { int32 r; *this >> r; return r; }
	GE_INLINE uint32 ReadUint32() { uint32 r; *this >> r; return r; }
	GE_INLINE int64 ReadInt64() { int64 r; *this >> r; return r; }
	GE_INLINE uint64 ReadUint64() { uint64 r; *this >> r; return r; }
	GE_INLINE float ReadFloat() { float r; *this >> r; return r; }
	GE_INLINE double ReadDouble() { double r; *this >> r; return r; }

	GE_INLINE size_t GetSize() { return size(); }

	int GetReadPos()
	{
		return rpos();
	}

	void SetReadPos(size_t pos)
	{
		rpos(pos);
	}

	void ReadByte( char* dest, int len )
	{
		this->read( (uint8*)dest, len );
	}

	string GetContents()//把内容转化成为字符串
	{
		char* data = (char*)this->contents();
		int size = this->size();
		string str = "";
		for( int i = 0; i < size; ++i )
		{
			str += data[i];
		}
		return str;
//		cout << "contents = " << worldPacket.contents() << " size = " << worldPacket.size() << endl;
	}

	const uint8* GetRemainByte()
	{
//		std::cout << "post = " << this->GetReadPos() << endl;
		int pos =this->GetReadPos();
		if(pos < 0 )
		{
			pos = 0;
		}
		return &_storage[pos];
	}

	string ReadUnicode()
	{
		size_t len = this->ReadUint16();
		if(len < 65535)
		{
			uint8 buf[65535] = {};
			this->read(buf, len);
			return string((char*)buf, len);
		}
		LOG(ERROR)<<"读取unicode字符串失败，长度非法 len:"<<len<<endl;
		return "";
	}

	void WriteUnicode(const string& str)
	{
		uint16 len = str.length();
		*this << (uint16)(str.length());
		append((uint8 *)str.c_str(), len);
	}

public:
    GE_INLINE WorldPacket() : ByteBuffer(), m_opcode(0) { }
    GE_INLINE WorldPacket(uint32 opcode, size_t res) : ByteBuffer(res), m_opcode(opcode) {}
    GE_INLINE WorldPacket(size_t res) : ByteBuffer(res), m_opcode(0) { }
    GE_INLINE WorldPacket(const WorldPacket &packet) : ByteBuffer(packet), m_opcode(packet.m_opcode), m_id(packet.m_id) {}

    //! Clear packet and set opcode all in one mighty blow
    GE_INLINE void Initialize(uint32 opcode )
    {
        clear();
        m_opcode = opcode;
    }

    GE_INLINE uint32 GetOpcode() const { return m_opcode; }
    GE_INLINE void SetOpcode(uint32 opcode) {
    	Initialize(opcode);
    }
    GE_INLINE void SendMsgIni(uint32 opcode) { Initialize(opcode); }
	GE_INLINE void SendSuccessMsgIni(uint32 opcode) { Initialize(opcode); *this<<(uint32)0; }
	GE_INLINE void SendErrorMsgIni(uint32 opcode, uint32 errCode) { Initialize(opcode); *this<<errCode; }

	GE_INLINE void SendUserSuccessMsgIni(uint32 opcode, int fd) { Initialize(opcode); *this<<fd<<(uint32)0; }
	GE_INLINE void SendUserErrorMsgIni(uint32 opcode, int fd, uint32 errCode) { Initialize(opcode); *this<<fd<<errCode; }

//	GE_INLINE void SendSSMsgIni(uint32 opcode) { Initialize(opcode); *this<<(uint32)opcode; }
//	GE_INLINE void SendSSMsgSuccessIni(uint32 opcode) { Initialize(opcode); *this<<(uint32)opcode; *this<<(uint32)0; }
	void SendChangeOpcode(int opcode)
	{
		m_opcode = opcode;
		int nowSize = this->GetSize();
		this->WriteInt32( 0, 0 );
		int pos = this->GetReadPos();
		int writeSize = nowSize - pos;

		const uint8* remainByte = GetRemainByte();
		uint8 tempChar[DEFAULT_SIZE];
		memcpy(tempChar, remainByte, writeSize);

		cout << "pos = " << pos << " writeSize = " << writeSize << endl;
		put( 4, tempChar, writeSize );
		forceResize( nowSize - 4 );
	}


	int GetWritePos(){ return wpos(); };

	GE_INLINE uint16 GetId() { return m_id; }
	GE_INLINE void SetId(uint16 id) { m_id = id; }

	void ReadInt32Vec(int count, std::vector<int>& vec)
	{
		int len = count * sizeof(int);
		if(GetReadPos() + len > (int)GetSize())
		{
			return;
		}
		memmove(vec.data(), contents() + GetReadPos(), len);
		rpos(rpos() + len);
	}
protected:
    uint32 m_opcode;
    uint16 m_id = 0;
};

//template<uint32 Size>
//class SERVER_DECL StackWorldPacket : public StackBuffer<Size>
//{
//	uint16 m_opcode;
//public:
//	GE_INLINE StackWorldPacket(uint16 opcode) : StackBuffer<Size>(), m_opcode(opcode) { }
//
//	//! Clear packet and set opcode all in one mighty blow
//	GE_INLINE void Initialize(uint16 opcode )
//	{
//		StackBuffer<Size>::Clear();
//		m_opcode = opcode;
//	}
//
//	uint16 GetOpcode() { return m_opcode; }
//	GE_INLINE void SetOpcode(uint16 opcode) { m_opcode = opcode; }
//};

#endif
