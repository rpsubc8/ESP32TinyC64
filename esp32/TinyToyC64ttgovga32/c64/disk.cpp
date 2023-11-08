/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42): Rink Springer <rink@rink.nu> wrote
 * this file. As long as you retain this notice you can do whatever you want
 * with this stuff. If we meet some day, and you think this stuff is worth it,
 * you can buy me a beer in return Rink Springer
 * ----------------------------------------------------------------------------
 */
#include "disk.h"
#include <assert.h>
#include <stdio.h>

#if 1
#define TRACE(x...)
#else
#define TRACE(x...) printf(x)
#endif

unsigned int Disk::s_sector_size = 256;
unsigned int Disk::s_sectors_per_track[42] = {
	/* Tracks 1..17 contain 21 sectors each */
	21, 21, 21, 21, 21, 21, 21, 21,
	21, 21, 21, 21, 21, 21, 21, 21,
	21,
	/* Tracks 18..24 contain 19 sectors each */
	19, 19, 19, 19, 19, 19, 19,
	/* Tracks 25..30 contain 18 sectors each */
	18, 18, 18, 18, 18, 18,
	/* Tracks 31..35 contain 17 sectors each */
	17, 17, 17, 17, 17,
	/* Tracks 36..42 contain 17 sectors each (non standard!) */
	17, 17, 17, 17, 17, 17, 17
};

unsigned int Disk::CalculateOffset(unsigned int track, unsigned int sector)
{
	assert(track >= 1 && track <= sizeof(s_sectors_per_track) / sizeof(s_sectors_per_track[0]));
	assert(sector >= 0 && sector < s_sectors_per_track[track - 1]);

	/* XXX This is really stupid and O(|sectors_per_track|) - we should be able to do better! */
	unsigned int offset = 0;
	for (unsigned int t = 1; t < track; t++) {
		offset += s_sectors_per_track[t - 1] * s_sector_size;
	}
	offset += sector * s_sector_size;
	return offset;
}

Disk::Disk()
	: m_data(NULL)
{
	// Sanity checks; these ensure out track/sector -> offset mapping is correct
#define VERIFY_TRACK_SECTOR_OFFSET(track, sector, offset) \
	assert(CalculateOffset((track), (sector)) == (offset) * s_sector_size)

	VERIFY_TRACK_SECTOR_OFFSET( 1,  0,   0);
	VERIFY_TRACK_SECTOR_OFFSET( 1, 20,  20);
	VERIFY_TRACK_SECTOR_OFFSET( 2,  0,  21);
	VERIFY_TRACK_SECTOR_OFFSET( 2, 20,  41);
	VERIFY_TRACK_SECTOR_OFFSET( 3,  0,  42);
	VERIFY_TRACK_SECTOR_OFFSET(18,  0, 357);
	VERIFY_TRACK_SECTOR_OFFSET(35,  0, 666);
	VERIFY_TRACK_SECTOR_OFFSET(35, 16, 682);

#undef VERIFY_TRACK_SECTOR_OFFSET
}

Disk::~Disk()
{
	delete[] m_data;
}

bool Disk::Load(const char* fname)
{
	// Throw away the previous disk first, if any
	delete[] m_data;
	m_data = NULL;

	FILE* f = fopen(fname, "rb");
	if (f == NULL)
		return false;

	// Figure out the disk size
	fseek(f, 0, SEEK_END);
	unsigned int len = ftell(f);
	rewind(f);

	// Use the disk size to determine how many tracks we have
	//JJ m_num_tracks = -1;
	m_num_tracks = 0; //JJ ponemos 0
	for (int n = 35; n < 40; n++) {
		if (CalculateOffset(n, s_sectors_per_track[n] - 1) + s_sector_size != len)
			continue;
		m_num_tracks = n;
		break;
	}
	if (m_num_tracks < 0) {
		// Can't figure out the number of tracks; likely not a D64 disk
		fclose(f);
		return false;
	}

	// Success; read the disk
	m_data = new uint8_t[len];	
	if (!fread(m_data, len, 1, f)) {
		fclose(f);
		return false;
	}
	fclose(f);

	//JJ TRACE("Disk::Load(): loaded '%s', %u tracks\n", fname, m_num_tracks);

	Dump();
	return true;
}

void Disk::Dump()
{
	for (int n = 0; /* nothing */; n++) {
		Entry entry;
		if (!GetDirectoryEntry(n, entry))
			break;
		entry.Dump();
	}
}

Disk::Entry::Entry()
{
}

Disk::Entry::Entry(const uint8_t* pDirEntry)
{
	const uint8_t* p = pDirEntry;
	m_next_dir_track = *p++;
	m_next_dir_sector = *p++;
	m_type = *p++;
	m_first_track = *p++;
	m_first_sector = *p++;
	for (unsigned int n = 0; n < sizeof(m_name) - 1; n++)
		m_name[n] = *p++;
	m_rel_first_track = *p++;
	m_rel_first_sector = *p++;
	m_rel_record_len = *p++;
	p += 6; // skip unused fields
	m_length = *p++;
	m_length |= *p++ << 8;
	assert(p - pDirEntry == s_entry_size);
}

bool Disk::Entry::Matches(const char* name) const
{
	const char* cur = m_name;
	for (/* nothing */; *name != '\0'; name++, cur++) {
		if (*name == '?')
			continue;
		if (*name == '*')
			return true;
		if (*name != *cur)
			return false;
	}

	// If we got here, we hit the end of the search string; this means the
	// entry name must end too
	return *cur == '\0';
}

uint8_t* Disk::GetDataPointer(unsigned int track, unsigned int sector)
{
	return (uint8_t*)(m_data + CalculateOffset(track, sector));
}

const uint8_t* Disk::GetDataPointer(unsigned int track, unsigned int sector) const
{
	return (const uint8_t*)(const_cast<Disk*>(this)->GetDataPointer(track, sector));
}


bool Disk::GetDirectoryEntry(int n, Entry& entry) const
{
	unsigned int cur_dir_track = 18, cur_dir_sector = 1;
	unsigned int next_dir_track, next_dir_sector;

	int cur_entry = 0;
	const int entries_per_sector = s_sector_size / Entry::s_entry_size;
	while (cur_dir_track > 0 && cur_dir_track <= m_num_tracks) {
		// Obtain the current entry
		const uint8_t* entryptr = (const uint8_t*)(GetDataPointer(cur_dir_track, cur_dir_sector) + cur_entry * Entry::s_entry_size);
		entry = Entry(entryptr);
		if (n == 0)
			return true;

		// For each entry on a boundary, store the next directory track/sector
		if (cur_entry % entries_per_sector == 0)
			entry.GetNextDirectoryTrackSector(next_dir_track, next_dir_sector);

		if (cur_entry % entries_per_sector == (entries_per_sector - 1)) {
			// At the end of this sector; try the next one
			cur_dir_track = next_dir_track;
			cur_dir_sector = next_dir_sector;
		}
		cur_entry++;
	} while(n-- > 0);

	return false;
}

bool Disk::FindDirectoryEntry(const char* name, Entry& entry) const
{
	for (int n = 0; /* nothing */; n++) {
		if (!GetDirectoryEntry(n, entry))
			return false;

		// See if the name matches, one char at a time
		if (entry.Matches(name))
			return true;
	}

	/* NOTREACHED */
}

bool Disk::OpenFile(const char* name, Handle& handle)
{
	if (!FindDirectoryEntry(name, handle.m_entry)) {
		handle.m_disk = NULL; // invalidates handle
		return false;
	}

	handle.m_disk = this;
	handle.m_file_offset = 0;
	handle.m_next_track = handle.m_entry.m_first_track;
	handle.m_next_sector = handle.m_entry.m_first_sector;
	return true;
}

Disk::Handle::Handle()
	: m_disk(NULL)
{
}

bool Disk::Handle::IsValid() const
{
	return m_disk != NULL;
}

bool Disk::Handle::AtEOF() const
{
	return m_file_offset >= m_entry.m_length * Disk::s_sector_size;
}

uint8_t Disk::Handle::GetNextByte()
{
	assert(m_file_offset < m_entry.m_length * Disk::s_sector_size);

	if (m_file_offset % Disk::s_sector_size == 0) {
		m_cur_track = m_next_track;
		m_cur_sector = m_next_sector;
		//JJ TRACE("Disk::Handle::GetNextByte(): cur track/sector = %u/%u\n", m_cur_track, m_cur_sector);
	}

	const uint8_t* data = m_disk->GetDataPointer(m_cur_track, m_cur_sector);
	data += m_file_offset % Disk::s_sector_size;
	if (m_file_offset % Disk::s_sector_size == 0) {
		// At the start of the block; store the next offset
		m_next_track = *data++;
		m_next_sector = *data++;
		//JJ TRACE("Disk::Handle::GetNextByte(): next track/sector = %u/%u\n", m_next_track, m_next_sector);
		m_file_offset += 2;
	}

	if (m_file_offset == Disk::s_sector_size - 1) {
		// End of this sector; need to prepare the second
		m_cur_track = m_next_track;
		m_cur_sector = m_next_sector;
	}

	//JJ TRACE("Disk::Handle::GetNextByte(): byte = 0x%x\n", *data);
	m_file_offset++;
	return *data;
}

void Disk::Entry::Dump() const
{
	printf("next dir entry: %u/%u\n", m_next_dir_track, m_next_dir_sector);
	printf("type: 0x%x\n", m_type);
	printf("first data entry: %u/%u\n", m_first_track, m_first_sector);
	printf("name: '%s'\n", m_name);
	printf("rel first entry: %u/%u\n", m_rel_first_track, m_rel_first_sector);
	printf("rel record length: %u\n", m_rel_record_len);
	printf("length: %u\n", m_length);
}

/* vim:set ts=2 sw=2: */
