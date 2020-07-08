#pragma once

#include <engine/shared/NetDataArray.h>
#include <base/CircularBuffer.h>
#include <base/Vector2D.h>
#include <engine/shared/NetServer.h>
#include <engine/shared/Snapshot.h>


class Server;

class ServerSnapshotManager {

private:

	struct Snapshot {
		unsigned int tick; // snapshot id
		unsigned int sequence;
		bool acked;
		bool empty;
		SnapshotData snapData;

		Snapshot() { Reset(); }

		void Reset() {
			tick = 0;
			sequence = 0;
			acked = false;
			empty = true;
			snapData.Reset();
		}
	};

	typedef CircularBuffer<Snapshot, 32> SnapshotHistory;

	static Snapshot m_NewSnap; // this is the pure last snap without management interest
	static Snapshot m_NewClientSnap; // client snap with management interested applied onto
	static SnapshotHistory m_ServerSnapHist;
	static NetDataArray<> m_CompressedDeltaSnap;
	static Server *m_Server;

	int m_ClientId;
	SnapshotHistory m_ClientSnapHist;

	static void DeltaCompress(const SnapshotData &from, const SnapshotData &to);
	void UpdateAckedSnapshots();
	const Snapshot * GetLastSnapshotAckedByClient() const;


	template <class T, class U>
	bool IsInAura(const U &auraOwner, float frameWidth, float frameHeight, const T &obj) const {
		Vector2D bottomLeft(auraOwner.x - frameWidth/2, auraOwner.y - frameHeight/2 );
		Vector2D topRight(auraOwner.x + frameWidth/2, auraOwner.y + frameHeight/2 );
		return  ! ((obj.x <= bottomLeft.x) || (obj.x >= topRight.x) || (obj.y <= bottomLeft.y) || (obj.y >= topRight.y) );
	}

	template <class T>
	void SetAuraSkipping(const T *objects, int numObjects, int clientId, float frameWidth, float frameHeight, byte_t *skip) const {
		std::memset(skip, 0, numObjects);
		for (int i = 0; i < numObjects; ++i)
			if (objects[i].exist && !IsInAura(m_NewSnap.snapData.netChars[clientId], frameWidth, frameHeight, objects[i]) )
				skip[i] = 1;
	}

public:

	void Reset();
	static void GenerateSnapshot();
	void GenerateClientBasedSnapshot();
	static inline NetDataArray<> & GetLastDeltaSnapCompressed();
	static void SetServer(Server *server) {m_Server = server;}
	void Update();
	void SaveLastClientSnapshot(unsigned int sequence);
	void SetClientId(int clientId) {m_ClientId = clientId;}
};

inline NetDataArray<> & ServerSnapshotManager::GetLastDeltaSnapCompressed() {
	return m_CompressedDeltaSnap;
}
