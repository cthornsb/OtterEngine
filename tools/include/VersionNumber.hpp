#ifndef VERSION_NUMBER_HPP
#define VERSION_NUMBER_HPP

#include <string>

namespace ott {

	class VersionNumber {
	public:
		/** Default constructor (version 0.0.0)
		  */
		VersionNumber() :
			nVersionMajor(0),
			nVersionMinor(0),
			nVersionRevision(0)
		{
		}

		/** Version string constructor.
		  * Expects a version string of the form "MM.mm.rr Other text may follow after a space",
		  * where MM is the major version number, mm is the minor, and rr is the revision number.
		  * Note that the minor and revision version numbers are optional and that text may follow
		  * the version after a space.
		  */
		VersionNumber(const std::string& versionStr) :
			VersionNumber()
		{
			get(versionStr, *this);
		}

		/** Explicit version number constructor
		  */
		VersionNumber(const int& major, const int& minor, const int& rev=0) :
			nVersionMajor(major),
			nVersionMinor(minor),
			nVersionRevision(rev)
		{
		}

		/** Return true if the version is greater than or equal to a specified version
		  */
		bool operator >= (const VersionNumber& rhs) const;

		/** Return true if the version is greater than or equal to a specified version
		  */
		bool operator <= (const VersionNumber& rhs) const;

		/** Return true if the version is greater than or equal to a specified version
		  */
		bool operator == (const VersionNumber& rhs) const;

		/** Get the major version number
		  */
		int getMajorVersion() const {
			return nVersionMajor;
		}

		/** Get the minor version number
		  */
		int getMinorVersion() const {
			return nVersionMinor;
		}

		/** Get the version revision number
		  */
		int getRevision() const {
			return nVersionRevision;
		}

		/** Get the version number string
		  */
		std::string getString() const;

		/** Get a version number from an input string
		  */
		static void get(const std::string& str, VersionNumber& version);

	private:
		int nVersionMajor; ///< Major version number

		int nVersionMinor; ///< Minor version number

		int nVersionRevision; ///< Revision number
	};

}; // namespace ott

#endif