#include "ServerSnapshotManager.h"
#include <engine/server/Server.h>
#include <engine/shared/Snapshot.h>
#include <game/server/Projectile.h>
#include <game/server/Map.h>
#include <engine/shared/AbsDim.h>

ServerSnapshotManager::Snapshot ServerSnapshotManager::m_NewSnap;
ServerSnapshotManager::Snapshot ServerSnapshotManager::m_NewClientSnap;
ServerSnapshotManager::SnapshotHistory ServerSnapshotManager::m_ServerSnapHist;
NetDataArray<> ServerSnapshotManager::m_CompressedDeltaSnap;
Server *ServerSnapshotManager::m_Server;

void ServerSnapshotManager::Reset() {
	for (int i = 0; i < 32; ++i) {
		m_ClientSnapHist[i].acked = false;
		m_ClientSnapHist[i].empty = true;
		m_ClientSnapHist[i].sequence = 0;
		m_ClientSnapHist[i].snapData.Reset();
	}
	m_ClientSnapHist.Reset();
}

const ServerSnapshotManager::Snapshot * ServerSnapshotManager::GetLastSnapshotAckedByClient() const {
	for (int i=31; i>=0; --i)
		if (m_ClientSnapHist[i].acked)
			return &m_ClientSnapHist[i];
	return 0;
}

void ServerSnapshotManager::GenerateClientBasedSnapshot() {

	float frameWidth = AbsDim::TILE_SIZE * 55;
	float frameHeight = AbsDim::TILE_SIZE * 33;
	const int MAX_NET_OBJECTS = 256;
	byte_t skip[MAX_NET_OBJECTS];

	const Snapshot *lastClientSnap = GetLastSnapshotAckedByClient();

	if (!lastClientSnap) {
		Snapshot dummy;
		std::memset(&dummy, 0, sizeof(Snapshot));
		lastClientSnap = &dummy;
	}

	bool charExist[MAX_CLIENTS];
	for (int i = 0; i < MAX_CLIENTS; ++i)
		charExist[i] = m_NewClientSnap.snapData.netChars[i].exist;
	bool projExist[MAX_PROJECTILES];
	for (int i = 0; i < MAX_PROJECTILES; ++i)
		projExist[i] = m_NewClientSnap.snapData.netProjs[i].exist;

	m_NewClientSnap = *lastClientSnap;

	for (int i = 0; i < MAX_CLIENTS; ++i)
		m_NewClientSnap.snapData.netChars[i].exist = charExist[i];
	for (int i = 0; i < MAX_PROJECTILES; ++i)
		m_NewClientSnap.snapData.netProjs[i].exist = projExist[i];

	// --- CHARACTERS ---

	SetAuraSkipping(m_NewSnap.snapData.netChars, MAX_CLIENTS, m_ClientId, frameWidth, frameHeight, skip);

	std::memset(skip, 0, 256);
	std::vector<Character*> &chars = m_Server->m_GameServer.Characters();
	for (int i = 0; i < MAX_CLIENTS; ++i)
		if (!skip[i])
			chars[i]->GetSnapshot(m_NewClientSnap.snapData.netChars[i]);

	// --- PROJECTILES ---

	SetAuraSkipping(m_NewSnap.snapData.netProjs, MAX_PROJECTILES, m_ClientId, frameWidth, frameHeight, skip); // FIXME ne pas tout parcourir !
	std::list<Projectile*> &projs = m_Server->m_GameServer.Projectiles();
	std::list<Projectile*>::const_iterator it = projs.begin();
	while (it != projs.end())
	{
		Projectile *pProj = *it;
		int id = pProj->ArrayId();
		if (!skip[id]) {
			pProj->GetSnapshot(m_NewClientSnap.snapData.netProjs);
		}
		++it;
	}

	// --- TRIGGERS ---

	SetAuraSkipping(m_NewSnap.snapData.netTrigs, MAX_TRIGGERS, m_ClientId, frameWidth, frameHeight, skip);
	m_Server->m_GameServer.GetMap()->TriggerSys().GetSnapshot(m_NewClientSnap.snapData.netTrigs, skip);


	// --- PLAYERS ---

	const GameServer::ClientStats *cs = m_Server->GetGameServer().GetClientStats();
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		m_NewClientSnap.snapData.netPlayers[i].kills = cs[i].kills;
		m_NewClientSnap.snapData.netPlayers[i].deaths = cs[i].deaths;
	}

}

void ServerSnapshotManager::GenerateSnapshot() {

	// --- CLIENTS ---

	std::vector<Character*> &chars = m_Server->m_GameServer.Characters();
	for (int i = 0; i < MAX_CLIENTS; i++) {
		chars[i]->GetSnapshot(m_NewSnap.snapData.netChars[i]);
	}

	// --- PROJECTILES ---

	std::list<Projectile*> &projs = m_Server->m_GameServer.Projectiles();
	std::list<Projectile*>::const_iterator it = projs.begin();
	while (it != projs.end())
	{
		Projectile *pProj = *it;
		pProj->GetSnapshot(m_NewSnap.snapData.netProjs);
		++it;
	}

	// --- TRIGGERS ---

	byte_t skip[MAX_TRIGGERS] = {0}; // skip nothing
	m_Server->m_GameServer.GetMap()->TriggerSys().GetSnapshot(m_NewClientSnap.snapData.netTrigs, skip);

	m_ServerSnapHist.Add(m_NewSnap);


	const GameServer::ClientStats *cs = m_Server->GetGameServer().GetClientStats();
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		m_NewClientSnap.snapData.netPlayers[i].kills = cs[i].kills;
		m_NewClientSnap.snapData.netPlayers[i].deaths = cs[i].deaths;
	}
}

void ServerSnapshotManager::Update() {

	UpdateAckedSnapshots();
	GenerateClientBasedSnapshot(); // can filter objects under some conditions
	const Snapshot *lastSnapAcked = GetLastSnapshotAckedByClient();

	if (lastSnapAcked)
	{
		DeltaCompress(lastSnapAcked->snapData, m_NewClientSnap.snapData);
	}
	else
	{
		SnapshotData dummy;
		std::memset(&dummy, 0, sizeof(SnapshotData));
		DeltaCompress(dummy, m_NewClientSnap.snapData);
	}
}

void ServerSnapshotManager::UpdateAckedSnapshots() {

	const std::list<unsigned int> & ackedSequences = m_Server->m_NetServer.GetAckedSequences(m_ClientId);

	for (int i = 31; i >= 0; --i) {

		if (m_ClientSnapHist[i].empty)
			continue;

		if (m_ClientSnapHist[i].acked)
			return;

		// search for sequence in acked sequence
		for (std::list<unsigned int>::const_iterator it = ackedSequences.begin(); it != ackedSequences.end(); ++it) {
			unsigned int ackedSequence = *it;
			if (m_ClientSnapHist[i].sequence == ackedSequence) {
				m_ClientSnapHist[i].acked = true;
				return;
			}
		}
	}
}

void ServerSnapshotManager::DeltaCompress(const SnapshotData &from, const SnapshotData &to) {

	m_CompressedDeltaSnap.Rewind();

	NetDataArray<MAX_UNCOMPRESSED_SIZE> uncompressedData;
	NetDataArray<> compressedData;

	// --- CHARACTERS ---

	for (int i = 0; i < MAX_CLIENTS; ++i)
		SnapshotHelper::WriteDeltaObject(&from.netChars[i], &to.netChars[i], SnapshotData::charFields, SnapshotData::numCharFields, uncompressedData);
	SnapshotHelper::CompressObjects(uncompressedData, MAX_CLIENTS, SnapshotData::charFields, SnapshotData::numCharFields, compressedData);
	m_CompressedDeltaSnap.Add(compressedData);

	// --- PLAYERS ---

	uncompressedData.Rewind();
	compressedData.Rewind();
	for (int i = 0; i < MAX_CLIENTS; ++i)
		SnapshotHelper::WriteDeltaObject(&from.netPlayers[i], &to.netPlayers[i], SnapshotData::playFields, SnapshotData::numPlayFields, uncompressedData);
	SnapshotHelper::CompressObjects(uncompressedData, MAX_CLIENTS, SnapshotData::playFields, SnapshotData::numPlayFields, compressedData);
	m_CompressedDeltaSnap.Add(compressedData);

	// --- PROJECTILES ---

	uncompressedData.Rewind();
	compressedData.Rewind();
	for (int i = 0; i < MAX_PROJECTILES; ++i)
		SnapshotHelper::WriteDeltaObject(&from.netProjs[i], &to.netProjs[i], SnapshotData::projFields, SnapshotData::numProjFields, uncompressedData);
	SnapshotHelper::CompressObjects(uncompressedData, MAX_PROJECTILES, SnapshotData::projFields, SnapshotData::numProjFields, compressedData);
	m_CompressedDeltaSnap.Add(compressedData);

	// --- TRIGGERS ---

	uncompressedData.Rewind();
	compressedData.Rewind();
	for (int i = 0; i < MAX_TRIGGERS; ++i)
		SnapshotHelper::WriteDeltaObject(&from.netTrigs[i], &to.netTrigs[i], SnapshotData::trigFields, SnapshotData::numTrigFields, uncompressedData);
	SnapshotHelper::CompressObjects(uncompressedData, MAX_TRIGGERS, SnapshotData::trigFields, SnapshotData::numTrigFields, compressedData);
	m_CompressedDeltaSnap.Add(compressedData);
}

void ServerSnapshotManager::SaveLastClientSnapshot(unsigned int sequence) {
	m_ClientSnapHist.Add(m_NewClientSnap);
	Snapshot & lastSnap = m_ClientSnapHist.Last();
	lastSnap.sequence = sequence;
	lastSnap.acked = false;
	lastSnap.empty = false;
}
