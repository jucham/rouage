#include "ClientSnapshotManager.h"

void ClientSnapshotManager::Reset() {
	m_LastSnapData.Reset();
}

void ClientSnapshotManager::DeltaUncompress(NetPiece &netPiece) {

	NetDataArray<MAX_UNCOMPRESSED_SIZE> uncompressedObjects;
	SnapshotHelper::UncompressObjects(netPiece, MAX_CLIENTS, SnapshotData::charFields, SnapshotData::numCharFields, uncompressedObjects);
	for (int i = 0; i < MAX_CLIENTS; ++i)
		SnapshotHelper::ReadDeltaObject(uncompressedObjects, SnapshotData::charFields, SnapshotData::numCharFields, &m_LastSnapData.netChars[i]);

	uncompressedObjects.Rewind();
	SnapshotHelper::UncompressObjects(netPiece, MAX_CLIENTS, SnapshotData::playFields, SnapshotData::numPlayFields, uncompressedObjects);
	for (int i = 0; i < MAX_CLIENTS; ++i)
		SnapshotHelper::ReadDeltaObject(uncompressedObjects, SnapshotData::playFields, SnapshotData::numPlayFields, &m_LastSnapData.netPlayers[i]);

	uncompressedObjects.Rewind();
	SnapshotHelper::UncompressObjects(netPiece, MAX_PROJECTILES, SnapshotData::projFields, SnapshotData::numProjFields, uncompressedObjects);
	for (int i = 0; i < MAX_PROJECTILES; ++i)
		SnapshotHelper::ReadDeltaObject(uncompressedObjects, SnapshotData::projFields, SnapshotData::numProjFields, &m_LastSnapData.netProjs[i]);

	uncompressedObjects.Rewind();
	SnapshotHelper::UncompressObjects(netPiece, MAX_TRIGGERS, SnapshotData::trigFields, SnapshotData::numTrigFields, uncompressedObjects);
	for (int i = 0; i < MAX_TRIGGERS; ++i)
		SnapshotHelper::ReadDeltaObject(uncompressedObjects, SnapshotData::trigFields, SnapshotData::numTrigFields, &m_LastSnapData.netTrigs[i]);
}
