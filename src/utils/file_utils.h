/**
 * @file file_utils.h
 * @brief Provides an RAII wrapper for file-stream operations.
 *
 * This header defines the File class, which opens a file during construction
 * and automatically closes it when the object is destroyed. Supported modes
 * include reading, writing, combined reading and writing, and appending.
 *
 * Construction throws std::runtime_error if the requested file cannot be
 * opened.
 *
 * @date Created: 24/02/2015
 * @date Last updated: 23/09/2026
 */
#ifndef BITGRAPH_UTILS_FILE_LOG_H
#define BITGRAPH_UTILS_FILE_LOG_H

#include "utils/logger.h"

#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>

// MACRO preserved for backward compatibility with legacy code
// Preferably use:
// 
// bitgraph::utils::File log{
//	 filename,
//	 bitgraph::utils::File::Mode::append
//  };
//
// log.stream() << "message\n";

#define FILE_LOG(file,mode)									\
	::bitgraph::File(										\
		 (file), bitgraph::utils::File::Mode::mode).stream()		


namespace bitgraph {
	
	namespace utils {

		/**
		 * @brief RAII wrapper around std::fstream.
		 *
		 * The file is opened during construction and closed automatically when the
		 * File object is destroyed. File objects cannot be copied but can be moved.
		 */
		class File {
		public:

			/**
			 * @brief Supported file-opening modes.
			 */
			enum class Mode { READ = 0, WRITE, READ_WRITE, APPEND };


			/**
		   * @brief Opens a file using the requested mode.
		   *
		   * In read-write mode, the file is created if it does not already exist.
		   *
		   * @param filename Name or path of the file.
		   * @param mode File-opening mode.
		   * @throws std::runtime_error If the file cannot be opened.
		   */
			explicit File(
				const std::string& filename,
				Mode mode = Mode::APPEND)
			{
				open(filename, mode);				
			}

			// copy semantics forbidden, move semantics allowed, destructor closes the file stream
			File(const File&) = delete;
			File& operator=(const File&) = delete;

			File(File&&) noexcept = default;
			File& operator=(File&&) noexcept = default;

			~File() = default;


			/**
			 * @brief Returns the underlying file stream.
			 * @return Mutable reference to the file stream.
			 */
			const std::fstream& stream() const noexcept
			{
				return stream_;
			}

			/**
			* @brief Returns the underlying file stream.
			* @return Mutable reference to the file stream.
			*/
			std::fstream& stream() noexcept
			{
				return stream_;
			}

			/** @brief Returns whether the file stream is open. */
			bool is_open() const noexcept
			{
				return stream_.is_open();
			}

			/** @brief Returns whether the stream is in a good state. */
			bool good() const noexcept
			{
				return stream_.good();
			}


		private:
			void open(const std::string& filename, Mode mode)
			{
				switch (mode) {
				case Mode::READ:
					stream_.open(filename, std::ios::in);
					break;

				case Mode::WRITE:
					stream_.open(filename, std::ios::out);
					break;

				case Mode::READ_WRITE:
					stream_.open(filename, std::ios::in | std::ios::out);

					if (!stream_.is_open()) {
						stream_.clear();

						{
							std::ofstream create_file{ filename };
						}

						stream_.open(filename, std::ios::in | std::ios::out);
					}
					break;

				case Mode::APPEND:
					stream_.open(
						filename,
						std::ios::out | std::ios::app);
					break;
				}

				if (!stream_.is_open()) {
					LOGG_ERROR(
						"Unable to open file: ",
						filename,
						" - File::File");

					throw std::runtime_error{
						"File: unable to open '" + filename + "'" };
				}
			}

			//////////////
			// data members

			std::fstream stream_;
		};

	} // namespace utils

	// Preserves backward compatibility with legacy code
	using File = utils::File;

}//end namespace bitgraph

#endif  //BITGRAPH_UTILS_FILE_LOG_H
