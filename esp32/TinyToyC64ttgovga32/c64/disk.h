#ifndef __DISK_H__
#define __DISK_H__

#include <stdint.h>

//! \brief Contains a C64 disk
class Disk {
public:
	//! \brief Describes a directory entry
	class Entry {
		friend class Disk;
	public:
		Entry();

		//! \brief Retrieve the entry's name
		const char* GetName() const { return m_name; }
		unsigned int m_length; //Era privado, ahora publico
		//

	protected:
		//! \brief Fills an entry from a directory entry
		Entry(const uint8_t* pDirEntry);

		//! \brief Retrieve the next directory entry track/sector
		void GetNextDirectoryTrackSector(unsigned int& track, unsigned int& sector) {
			track = m_next_dir_track;
			sector = m_next_dir_sector;
		}

		//! \brief Dumps the entry to the console
		void Dump() const;

		/*! \brief Compare the entry name
		 *  \param name Name to compare to
		 *  \returns true on success
		 *
		 *  Wildcards * and ? are supported.
		 */
		bool Matches(const char* name) const;

		//! \brief Size of an on-disk directory entry, in bytes
		static const unsigned int s_entry_size = 32;

	private:
		//! \brief Next directory entry track/sector
		unsigned int m_next_dir_track, m_next_dir_sector;

		//! \brief Entry type
		unsigned int m_type;

		//! \brief First track/sector of entry content
		unsigned int m_first_track, m_first_sector;

		//! \brief Entry name, \0-terminated
		char m_name[17];

		//! \brief First track/sector of REL file
		unsigned int m_rel_first_track, m_rel_first_sector;

		//! \brief REL record length
		unsigned int m_rel_record_len;

		//! \brief Length in sectors
		//JJ unsigned int m_length; //No deberia ser privado
	};

	//! \brief Contains a handle to a file
	class Handle {
		friend class Disk;
	public:
		Handle();

		//! \brief Is the handle valid?
		bool IsValid() const;

		//! \brief Are we there yet?
		bool AtEOF() const;

		//! \brief Fetch the next byte
		uint8_t GetNextByte();

	protected:
		Handle(Disk* disk, Entry& entry);

		//! \brief Directory entry of this file
		Entry m_entry;

		//! \brief Current file offset;
		unsigned int m_file_offset;

		unsigned int m_cur_track, m_cur_sector;
		unsigned int m_next_track, m_next_sector;

		//! \brief Disk we belong to
		Disk* m_disk;
	};

	Disk();
	~Disk();

	/*! \brief Loads a disk from a given file
	 *  \param fname File to use
	 *  \returns true on success
	 */
	bool Load(const char* fname);

	//! \brief Dumps the disk information to the console
	void Dump();

	/*! \brief Open a given file
	 *  \param name File name to open
	 *  \param handle Handle to fill
	 *  \returns true on success
	 */
	bool OpenFile(const char* name, Handle& handle);

protected:
	/*! \brief Retrieve a given directory entry
	 *  \param n Entry to retrieve
	 *  \param entry Entry object to fill
	 *  \returns true on success
	 */
	bool GetDirectoryEntry(int n, Entry& entry) const;

	/*! \brief Finds a directory entry by name
	 *  \param name Name to look for
	 *  \param entry Entry to fill
	 *  \returns true on success
	 */
	bool FindDirectoryEntry(const char* name, Entry& entry) const;

	/*! \brief Fetch a given track/sector
	 *  \param track Track to use
	 *  \param sector Sector to use
	 *  \returns Data pointer
	 */
	uint8_t* GetDataPointer(unsigned int track, unsigned int sector);
	const uint8_t* GetDataPointer(unsigned int track, unsigned int sector) const;

	//! \brief Array mapping track number -> number of sectors for this track
	static unsigned int s_sectors_per_track[];

	//! \brief Sector size in bytes
	static unsigned int s_sector_size;

	//! \brief Calculates the D64 image offset for a given track/sector pair
	static unsigned int CalculateOffset(unsigned int track, unsigned int sector);

private:
	uint8_t* m_data;
	unsigned int m_num_tracks;
};

#endif /* __DISK_H__ */
