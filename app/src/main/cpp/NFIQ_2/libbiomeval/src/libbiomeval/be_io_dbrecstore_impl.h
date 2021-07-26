/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/
#ifndef __BE_DBRECSTORE_IMPL_H__
#define __BE_DBRECSTORE_IMPL_H__

#include <string>
#include <vector>

#include "be_io_recordstore_impl.h"
#include <be_io_dbrecstore.h>

#include <db_cxx.h>

/*
 * This file contains the class declaration for an implementation of a
 * RecordStore using a on-disk database.
 */
namespace BiometricEvaluation {

	namespace IO {

		/**
		 * @brief
		 * A class that implements IO::RecordStore using a Berkeley
		 * DB database as the underlying record storage system.
		 */
		class DBRecordStore::Impl : public RecordStore::Impl {
		public:

			/**
			 * Create a new DBRecordStore, read/write mode.
			 *
			 * @param[in] pathname
			 *	The directory where the store will be created.
			 * @param[in] description
			 *	The store's description.
			 *
			 * @throw Error::ObjectExists
			 * 	The store already exists.
			 * @throw Error::StrategyError
			 * 	An error occurred when accessing the underlying
			 * 	file system.
			 */
			Impl(
			    const std::string &pathname,
			    const std::string &description);

			/**
			 * Open an existing DBRecordStore.
			 *
			 * @param[in] name
			 *	The path name of the store.
			 * @param[in] mode
			 *	Open mode, read-only or read-write.
			 *
			 * @throw Error::ObjectDoesNotExist
			 *	The store does not exist.
			 * @throw Error::StrategyError
			 *	An error occurred when accessing the underlying
			 *	file system.
			 */
			Impl(
			    const std::string &pathname,
			    IO::Mode mode = IO::Mode::ReadOnly);

			/*
			 * Destructor.
			 */
			~Impl();

			/*
			 * Implementation of the RecordStore interface.
			 */
			uint64_t getSpaceUsed() const;

			void sync() const;

			Memory::uint8Array
			read(
			    const std::string &key) const;

			void insert(
			    const std::string &key,
			    const void *const data,
			    const uint64_t size);

			void remove(
			    const std::string &key);

			uint64_t length(
			    const std::string &key) const;

			void flush(
			    const std::string &key) const;

			RecordStore::Record sequence(int cursor);

			std::string
			sequenceKey(int cursor);

			void setCursorAtKey(
			    const std::string &key);

			void move(
			    const std::string &pathname);

			/* Prevent copying of DBRecordStore::Impl objects */
			Impl(const DBRecordStore::Impl&) = delete;
			Impl&
			    operator=(const DBRecordStore::Impl&) = delete;

		private:
			/* The file names of the underlying databases. */
			std::string _dbnameP;
			std::string _dbnameS;

			/*
			 * The handle to the underlying database for the
			 * primary segments of a record.
			 */
			std::shared_ptr<Db> _dbP;

			/*
			 * The handle for the underlying database for the
			 * non-primary (subordinate) segments.
			 */
			std::shared_ptr<Db> _dbS;

			/** Handle to cursor */
			std::shared_ptr<Dbc> _dbC{nullptr};

			/*
			 * Return the path to the underlying DB file.
			 */
			std::string getDBFilePathname() const;

			/*
			 * Indicator of whether the DB cursor has been
			 * initialized.
			 */
			bool _cursorIsInit{};

			/*
			 * Indicator of whether we are at the end of the
			 * record store.
			 */
			bool _atEnd{};

			/*
			 * Open the underlying database handle objects.
			 */
			void i_setup(
			    const std::string &pathname,
			    int dbFlags,
			    IO::Mode mode);

			/*
			 * Functions to insert/read/sequence/remove all
			 * segments of a record.
			 */
			void insertRecordSegments(const std::string &key,
			    const void *data, const uint64_t size);

			uint64_t readRecordSegments(
			    const std::string &key,
			    void *const data) const;

			void removeRecordSegments(const std::string &key);

			/**
			 * Internal implementation of sequencing through a
			 * store, returning the key, and optionally, the
			 * data.
			 * @param[in] returnData
			 * 	Whether to return the data with the key.
			 * @param[in] cursor
			 *	The location within the sequence of the
			 *	key/data pair to return.
			 * @return
			 *	The record that is next in sequence.
			 * @throw Error::ObjectDoesNotExist
			 *	End of sequencing.
			 * @throw Error::StrategyError
			 *	An error occurred when using the underlying
			 *	storage system.
			 */
			RecordStore::Record
			i_sequence(
			    bool returnData,
			    int cursor);
		};
	}
}
#endif	/* __BE_DBRECSTORE_IMPL_H__ */
