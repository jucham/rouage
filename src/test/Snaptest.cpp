/*
 * Snaptest.cpp
 *
 *  Created on: 22 mai 2014
 *      Author: fixmini
 */

// COMPILATION
// g++ -I../src engine/shared/Net.c base/tri_logger.cpp  engine/shared/NetDataArray.cpp Snaptest.cpp

// 1. Creez les netfields

// 2. Construire les snapshots
//  2.1 Construction avec zone mémoire toujours disponible
//  2.2 Construction avec zone mémoire réservéé

// 3. Générer de modifs de snapshots
//  3.1 modifs simple à la main
//  3.2 modifs aléatoires (OPT car peut prendre du temps)

// 4. Construire des delta snapshots

// 5. Vérifier taille minimale de snapshot pour un nouveau client
//  5.1 vérifier que le reset est bien fait quand une entité est inactive

// 6. Appliquer la compression RLE
//  6.1 compression/decompression

// 7. Appliquer l'interest management
//  7.1 simuler des entités inactives non visibles (un grand nombre)
//  7.2 vérifier que aucun MaJ n'est faite et que le snapshot et moins volumineux

//TODO remplacer les ifelse par des switch


//#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <engine/shared/NetDataArray.h>

typedef unsigned char byte_t;

struct entityState_t {
	float x;//4
	float y;//4
	int health;//4
	float speed;//4
	bool active;//1 17
	bool dead;//1

	void Init() {
		x = 123.4f;
		y = 456.7f;
		health = 100;
		speed = 0;
		active = false;
		dead = false;
	}

	void Print(const char *title = "notitle") {
		printf("%s\n", title);
		printf("x = %0.4f\n", x);
		printf("y = %0.4f\n", y);
		printf("health = %d\n", health);
		printf("speed = %0.4f\n", speed);
		printf("active = %d\n", active);
		printf("dead = %d\n", dead);
	}
};

#define MAX_ENTITIES 256
#define NDA_UNCOMPRESSED_SIZE 4096
#define	NETF(x) #x,(int)offsetof(struct entityState_t, x) // TODO faire fonction du type de netfield

struct NetField {
	const char *name;
	int	offset;
	int	bits;
	int type;

	enum {
		BOOL,
		BYTE,
		SHORT,
		INT,
		FLOAT
	};
};

entityState_t entities_before[MAX_ENTITIES];
entityState_t entities_after[MAX_ENTITIES];

// dans la pratique on aura un netfield pour les chars, un pour les projs, etc. du fait qu'ils ont des données différente
NetField entityStateFields[] = {
		{NETF(x), 0, NetField::FLOAT},
		{NETF(y), 0, NetField::FLOAT},
		{NETF(health), 2, NetField::SHORT},
		{NETF(speed), 0, NetField::FLOAT},
		{NETF(active), 1, NetField::BOOL},
		{NETF(dead), 1, NetField::BOOL}
};
int entityStateNumFields = sizeof(entityStateFields)/sizeof(entityStateFields[0]);


bool AddEntity(entityState_t *entities, const entityState_t &entity) {

	for (int i = 0; i < MAX_ENTITIES; ++i) {
		if (!entities[i].active) {
			entities[i].active = 1;
			entities[i].dead = entity.dead;
			entities[i].health = entity.health;
			entities[i].speed = entity.speed;
			entities[i].x = entity.x;
			entities[i].y = entity.y;
			printf("entity added at index %d\n", i);
			return true;
		}
	}
	printf("entity not added, misses space !\n");
	return false;
}


template <class T, class U>
bool IsFieldEquals(const T *e1, const T *e2, int fieldOffset) {
	U *v1 = (U *)( (byte_t *)e1 + fieldOffset );
	U *v2 = (U *)( (byte_t *)e2 + fieldOffset );
	if (*v1 != *v2)
		printf("field changed\n");
	return (*v1 == *v2);
}

template <class T, class U>
U GetFieldValue(const T *e, int fieldOffset) {
	return *((U *)( (byte_t *)e + fieldOffset ));
}

template <class T, class U>
void SetFieldValue(U value, const T *e, int fieldOffset) {
	U *pValue = ((U *)( (byte_t *)e + fieldOffset ));
	*pValue = value;
}

template <class T>
void WriteDeltaObject(const T *from, const T *to, const NetField *netFields, int numFields, NetDataArray<NDA_UNCOMPRESSED_SIZE> &nda) {

	int i;
	const NetField *field;
	for (i = 0, field = netFields ; i < numFields ; i++, field++ ) {

		bool fieldChanged = false;

		if (field->type == NetField::FLOAT) {
			if (!IsFieldEquals<T, int>(from, to, field->offset)) {
				float toVal = GetFieldValue<T, float>(to, field->offset);
				nda.AddByte(1);
				nda.AddFloat( toVal );
				fieldChanged = true;
			}
		} else if (field->type == NetField::INT) {
			if (!IsFieldEquals<T, int>(from, to, field->offset)) {
				int toVal = GetFieldValue<T, int>(to, field->offset);
				nda.AddByte(1);
				nda.AddInt32(toVal);
				fieldChanged = true;
			}
		} else if (field->type == NetField::SHORT) {
			if (!IsFieldEquals<T, short>(from, to, field->offset)) {
				short toVal = GetFieldValue<T, short>(to, field->offset);
				nda.AddByte(1);
				nda.AddInt16(toVal);
				fieldChanged = true;
			}
		} else if ( field->type == NetField::BYTE) {
			if (!IsFieldEquals<T, int>(from, to, field->offset)) {
				int toVal = GetFieldValue<T, int>(to, field->offset);
				nda.AddByte(1);
				nda.AddByte(toVal);
				fieldChanged = true;
			}
		}  else if ( field->type == NetField::BOOL) {
			if (!IsFieldEquals<T, bool>(from, to, field->offset)) {
				bool toVal = GetFieldValue<T, bool>(to, field->offset);
				nda.AddByte(1);
				nda.AddByte(toVal);
				fieldChanged = true;
			}
		} else {
			assert(false && "wrong bits number");
		}

		if (!fieldChanged)
			nda.AddByte((byte_t)0);
	}
}

template <class T>
void ReadDeltaObject(T *obj, const NetField *netFields, int numFields, NetDataArray<NDA_UNCOMPRESSED_SIZE> &nda) {

	int i;
	const NetField *field;
	for (i = 0, field = entityStateFields ; i < numFields ; i++, field++ ) {
		byte_t change = nda.GetByte(); // 0 no change, 1 change
		if (change) {
			switch (field->type) {
			case NetField::FLOAT:
				SetFieldValue<T, float>(nda.GetFloat(), obj, field->offset);
				break;
			case NetField::INT:
				SetFieldValue<T, int>(nda.GetInt32(), obj, field->offset);
				break;
			case NetField::SHORT:
				SetFieldValue<T, short>(nda.GetInt16(), obj, field->offset);
				break;
			case NetField::BYTE:
				SetFieldValue<T, byte_t>(nda.GetByte(), obj, field->offset);
				break;
			case NetField::BOOL:
				SetFieldValue<T, bool>(nda.GetByte(), obj, field->offset);
				break;
			default:
				assert(false && "wrong netfield type number");
			}
		}
	}
}

void WriteCompressedZeros(int numZeros, NetDataArray<> &dst) {
	printf("write %d compressed zeros\n", numZeros);
	dst.AddByte((byte_t)0);
	char digit[10];
	std::sprintf(digit, "%d$", numZeros);
	char *pDigit = digit;
	while (*pDigit) {
		dst.AddByte(*pDigit++);
	}
}

template <class T, class U>
void WriteField(const NetField *field, T &src, U &dst) {
	printf("write field: %s\n", field->name);
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

// compresser un array de d'objet d'un certain type, ici des entités
void CompressObjects(NetDataArray<NDA_UNCOMPRESSED_SIZE> &src, const NetField *netFields, int numFields, NetDataArray<> &dst) {

	int numZeros = 0;
	src.Rewind();

	for (int i = 0; i < MAX_ENTITIES; ++i) {
		//		printf("////// Compress Entity %d //////\n", i);
		int j;
		const NetField *field;
		for (j = 0, field = entityStateFields ; j < numFields ; j++, field++ ) {

			byte_t change = src.GetByte();

			if (!change) { // si 0
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

	// les données se termine par une suite de zeros
	if (numZeros > 0)
		WriteCompressedZeros(numZeros, dst);
}

int GetCompressedZeros(NetDataArray<> &src) {
	char digits[10];
	int i = 0;
	byte_t b = 0;
	while (b != '$') {
		b = src.GetByte();
		digits[i++] = b;
	}
	digits[i] = 0;
	return std::atoi(digits);
}

void WriteZeros(int numZeros, NetDataArray<NDA_UNCOMPRESSED_SIZE> &dst) {
	printf("write %d zeros\n", numZeros);
	byte_t zeros[numZeros];
	std::memset(zeros, 0, numZeros);
	dst.AddBytes(zeros, numZeros);
}

void UncompressObjects(NetDataArray<> &src, const NetField *netFields, int numFields, NetDataArray<NDA_UNCOMPRESSED_SIZE> &dst) {

	int fieldsToSkip = 0;
	src.Rewind();

	for (int i = 0; i < MAX_ENTITIES; ++i) { //TODO mettre le max en param
		//		printf("////// Uncompress Entity %d //////\n", i);
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
				const int numZeros = GetCompressedZeros(src);
				WriteZeros(numZeros, dst);
				fieldsToSkip = numZeros-1;
			} else {
				WriteField(field, src, dst);
			}
		}
	}
}

int main() {
	printf("=============================\n");

	//	printf("sizeof(int): %ld\n", sizeof(int));
	printf("sizeof(entityState_t) = %ld\n", sizeof(entityState_t));
	//	printf("sizeof(netFields) = %ld\n", sizeof(netFields) / sizeof(NetField));
	//	for (int i = 0; i < 6; ++i) {
	//		printf("name : %s, offset : %d, bits : %d\n", netFields[i].name, netFields[i].offset, netFields[i].bits);


	// --- INIT ENTITIES ---
	int k=40;
	for (int i = 0; i < MAX_ENTITIES; ++i) {
		entities_before[i].Init();
		entities_after[i].Init();

		//		if (k>0) {
		//			k--;
		//			entities_after[i].x = 0.987f;
		//			entities_after[i].y = 0.987f;
		//			entities_after[i].speed = 0.987f;
		//		}
	}


	entities_before[0].active = true;
	entities_after[0].active = false;
	entities_after[0].y = 10.235f;
	entities_after[0].speed = 999.888f;
	entities_after[0].dead = true;
	//	entities_after[0].x = 0.987f;
	//	entities_after[0].health = 22;
	//	entities_after[0].health = 50;
	//	entities_after[MAX_ENTITIES-1].dead = true;

	// --- WRITE DELTA ENTITIES ---

	printf("// --- WRITE DELTA ENTITIES ---\n");
	NetDataArray<NDA_UNCOMPRESSED_SIZE> ndaRaw;
	for (int i = 0; i < MAX_ENTITIES; ++i) {
		//		printf("###### Delta Entity %d ######\n", i);

		WriteDeltaObject(&entities_before[i], &entities_after[i], entityStateFields, entityStateNumFields, ndaRaw);
	}

	// --- COMPRESS ENTITIES ---

	printf("// --- COMPRESS ---\n");
	NetDataArray<> ndaCompressed;
	CompressObjects(ndaRaw, entityStateFields, entityStateNumFields, ndaCompressed);

	printf("raw size: %d\n", ndaRaw.GetSize());
	printf("compressed size: %d\n", ndaCompressed.GetSize());
	printf("ratio: %0.2lf %% of raw size.\n", (double)ndaCompressed.GetSize() / ndaRaw.GetSize() * 100);

	// --- UNCOMPRESS ENTITIES ---

	printf("// --- UNCOMPRESS ---\n");
	NetDataArray<NDA_UNCOMPRESSED_SIZE> ndaUncompressed;
	UncompressObjects(ndaCompressed, entityStateFields, entityStateNumFields, ndaUncompressed);

	int size = ndaRaw.GetSize();
	const byte_t *rawData = ndaRaw.GetData();
	const byte_t *uncompData = ndaUncompressed.GetData();
	bool success = true;
	for (int i = 0; i < size; ++i) {
		if (*rawData++ != *uncompData++) {
			success = false;
			break;
		}
	}

	if (success)
		printf("SUCCESS\n");
	else
		printf("FAIL\n");

	ndaUncompressed.Rewind();

	entityState_t freshEntity = entities_before[0];
	freshEntity.Print("Fresh Entity Before Update");
	ReadDeltaObject(&freshEntity, entityStateFields, entityStateNumFields, ndaUncompressed);
	freshEntity.Print("Fresh Entity After Update");

	return 0;
}
