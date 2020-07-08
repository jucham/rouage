#ifndef SNAPSHOTTOOL_H_
#define SNAPSHOTTOOL_H_

#include <cstddef>
#include <engine/shared/Common.h>
#include <engine/shared/NetDataArray.h>

static const int MAX_UNCOMPRESSED_SIZE = 4096;

#define NUM_FIELDS(x)(sizeof(x)/sizeof(x[0]))
#define	NETF(fieldname, structId) #fieldname,(int)offsetof(struct structId, fieldname)

struct NetField {
	const char *name;
	int	offset;
	int type;

	enum {
		BOOL,
		BYTE,
		STRING,
		SHORT,
		INT,
		FLOAT
	};
};

struct NetCharacter {
	bool exist;
	byte_t states; // general state and move state (4/4)
	float x;
	float y;
	byte_t weaponType;
	float weaponAngle;
	short ammo;
	byte_t health;
};

struct NetProjectile {
	bool exist;
	byte_t type;
	float x;
	float y;
	float angle;
};

struct NetTrigger {
	bool exist;
	byte_t type;
	byte_t subType;
	byte_t active;
	float x;
	float y;
};

struct NetPlayer {
	short kills;
	short deaths;
};

struct SnapshotData {

	static NetField charFields[];
	static NetField playFields[];
	static NetField projFields[];
	static NetField trigFields[];

	static int numCharFields;
	static int numPlayFields;
	static int numProjFields;
	static int numTrigFields;

	NetCharacter netChars[MAX_CLIENTS];
	NetPlayer netPlayers[MAX_CLIENTS];
	NetProjectile netProjs[MAX_PROJECTILES];
	NetTrigger netTrigs[MAX_TRIGGERS];

	SnapshotData() {
		Reset();
	}

	SnapshotData & operator=(const SnapshotData &sd) {
		std::memcpy(this->netChars, sd.netChars, sizeof(NetCharacter) * MAX_CLIENTS);
		std::memcpy(this->netPlayers, sd.netPlayers, sizeof(NetPlayer) * MAX_CLIENTS);
		std::memcpy(this->netProjs, sd.netProjs, sizeof(NetProjectile) * MAX_PROJECTILES);
		std::memcpy(this->netTrigs, sd.netTrigs, sizeof(NetTrigger) * MAX_TRIGGERS);
		return *this;
	}

	void Reset() {
		std::memset(this->netChars, 0, sizeof(NetCharacter) * MAX_CLIENTS);
		for(int i=0; i<MAX_CLIENTS; ++i) {
			this->netChars[i].weaponAngle = 0.0f;
		}

		std::memset(this->netPlayers, 0, sizeof(NetPlayer) * MAX_CLIENTS);
		std::memset(this->netProjs, 0, sizeof(NetProjectile) * MAX_PROJECTILES);
		std::memset(this->netTrigs, 0, sizeof(NetTrigger) * MAX_TRIGGERS);
	}
};

struct SnapshotHelper {

	template <class T, class U>
	static bool IsFieldEquals(const T *e1, const T *e2, int fieldOffset) {
		U *v1 = (U *)( (byte_t *)e1 + fieldOffset );
		U *v2 = (U *)( (byte_t *)e2 + fieldOffset );
		return (*v1 == *v2);
	}

	template <class T>
		static bool IsStringFieldEquals(const T *e1, const T *e2, int fieldOffset) {
			char **v1 = (char **)( (byte_t *)e1 + fieldOffset );
			char **v2 = (char **)( (byte_t *)e2 + fieldOffset );
			return IsEqualString(*v1,*v2);
		}

	template <class T, class U>
	static U GetFieldValue(const T *e, int fieldOffset) {
		return *((U *)( (byte_t *)e + fieldOffset ));
	}

	template <class T, class U>
	static void SetFieldValue(U value, const T *e, int fieldOffset) {
		U *pValue = ((U *)( (byte_t *)e + fieldOffset ));
		*pValue = value;
	}

	template <class T>
	static void WriteDeltaObject(const T *from, const T *to, const NetField *netFields, int numFields, NetDataArray<MAX_UNCOMPRESSED_SIZE> &dst) {

		int i;
		const NetField *field;
		for (i = 0, field = netFields ; i < numFields ; i++, field++ ) {
			bool fieldChanged = false;
			if (field->type == NetField::FLOAT) {
				if (!IsFieldEquals<T, int>(from, to, field->offset)) {
					float toVal = GetFieldValue<T, float>(to, field->offset);
					dst.AddByte(1);
					dst.AddFloat( toVal );
					fieldChanged = true;
				}
			} else if (field->type == NetField::INT) {
				if (!IsFieldEquals<T, int>(from, to, field->offset)) {
					int toVal = GetFieldValue<T, int>(to, field->offset);
					dst.AddByte(1);
					dst.AddInt32(toVal);
					fieldChanged = true;
				}
			} else if (field->type == NetField::SHORT) {
				if (!IsFieldEquals<T, short>(from, to, field->offset)) {
					short toVal = GetFieldValue<T, short>(to, field->offset);
					dst.AddByte(1);
					dst.AddInt16(toVal);
					fieldChanged = true;
				}
			} else if ( field->type == NetField::BYTE) {
				if (!IsFieldEquals<T, int>(from, to, field->offset)) {
					int toVal = GetFieldValue<T, int>(to, field->offset);
					dst.AddByte(1);
					dst.AddByte(toVal);
					fieldChanged = true;
				}
			}  else if ( field->type == NetField::BOOL) {
				if (!IsFieldEquals<T, bool>(from, to, field->offset)) {
					bool toVal = GetFieldValue<T, bool>(to, field->offset);
					dst.AddByte(1);
					dst.AddByte(toVal);
					fieldChanged = true;
				}
			}  else if ( field->type == NetField::STRING) {
				assert(false);

				} else {
				assert(false && "wrong bits number");
			}

			if (!fieldChanged)
				dst.AddByte((byte_t)0);
		}
	}

	template <class T, class U>
	static void ReadDeltaObject(U &src, const NetField *netFields, int numFields, T *obj) {

		int i;
		const NetField *field;
		for (i = 0, field = netFields ; i < numFields ; i++, field++ ) {
			byte_t change = src.GetByte(); // 0 no change, 1 change
			if (change) {
				switch (field->type) {
				case NetField::FLOAT:
					SetFieldValue<T, float>(src.GetFloat(), obj, field->offset);
					break;
				case NetField::INT:
					SetFieldValue<T, int>(src.GetInt32(), obj, field->offset);
					break;
				case NetField::SHORT:
					SetFieldValue<T, short>(src.GetInt16(), obj, field->offset);
					break;
				case NetField::BYTE:
					SetFieldValue<T, byte_t>(src.GetByte(), obj, field->offset);
					break;
				case NetField::BOOL:
					SetFieldValue<T, bool>(src.GetByte(), obj, field->offset);
					break;
				default:
					assert(false && "wrong netfield type number");
				}
			}
		}
	}

	static void WriteCompressedZeros(int numZeros, NetDataArray<> &dst) {
		dst.AddByte((byte_t)0);
		dst.AddInt16(numZeros);
	}

	template <class T, class U>
	static void WriteField(const NetField *field, T &src, U &dst) {
		dst.AddByte(1);
		if (field->type == NetField::FLOAT) {
			dst.AddFloat(src.GetFloat());
		} else if (field->type == NetField::INT) {
			dst.AddInt32(src.GetInt32());
		} else if (field->type == NetField::SHORT) {
			dst.AddInt16(src.GetInt16());
		} else if ( field->type == NetField::BYTE) {
			dst.AddByte(src.GetByte());
		}  else if ( field->type == NetField::BOOL) {
			dst.AddByte(src.GetByte());
		} else {
			assert(false && "wrong bits number");
		}
	}

	static void CompressObjects(NetDataArray<MAX_UNCOMPRESSED_SIZE> &src, int numObjects, const NetField *netFields, int numFields, NetDataArray<> &dst) {

		int numZeros = 0;
		src.Rewind();

		for (int i = 0; i < numObjects; ++i) {
			int j;
			const NetField *field;
			for (j = 0, field = netFields ; j < numFields ; j++, field++ ) {

				byte_t change = src.GetByte();

				if (!change) { // if 0
					numZeros++;
				} else { // 1 + value
					if (numZeros > 0) {
						WriteCompressedZeros(numZeros, dst);
						numZeros = 0;
					}
					WriteField(field, src, dst);
				}
			}
		}

		if (numZeros > 0)
			WriteCompressedZeros(numZeros, dst);
	}

	static int ReadCompressedZeros(NetDataArray<> &src) {
		return src.GetInt16();
	}

	static void WriteZeros(int numZeros, NetDataArray<MAX_UNCOMPRESSED_SIZE> &dst) {
		byte_t zeros[numZeros];
		std::memset(zeros, 0, numZeros);
		dst.AddBytes(zeros, numZeros);
	}

	static void UncompressObjects(NetDataArray<> &src, int numObjects, const NetField *netFields, int numFields, NetDataArray<MAX_UNCOMPRESSED_SIZE> &dst) {

		int fieldsToSkip = 0;

		for (int i = 0; i < numObjects; ++i) {
			int j;
			const NetField *field;
			for (j = 0, field = netFields ; j < numFields ; j++, field++ ) {

				if (fieldsToSkip) {
					fieldsToSkip--;
					continue;
				}

				byte_t change = src.GetByte();
				assert(change==0 || change==1);

				if (!change) {
					const int numZeros = ReadCompressedZeros(src);
					WriteZeros(numZeros, dst);
					fieldsToSkip = numZeros-1;
				} else {
					WriteField(field, src, dst);
				}
			}
		}

		dst.Rewind();
	}
};

#endif /* SNAPSHOTTOOL_H_ */
