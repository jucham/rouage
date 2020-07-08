#ifndef CLIENTSNAPSHOTMANAGER_H_
#define CLIENTSNAPSHOTMANAGER_H_

#include <engine/shared/Snapshot.h>
#include <engine/shared/NetPiece.h>

class ClientSnapshotManager {

private:
	SnapshotData m_LastSnapData;

public:
	void Reset();
	void DeltaUncompress(NetPiece &netPiece);
	inline const SnapshotData & GetLastSnapshotData() {return m_LastSnapData;}

};

#endif /* CLIENTSNAPSHOTMANAGER_H_ */
