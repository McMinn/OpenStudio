/**********************************************************************
 *  Copyright (c) 2008-2013, Alliance for Sustainable Energy.
 *  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **********************************************************************/

#ifndef PROJECT_URLRECORD_IMPL_HPP
#define PROJECT_URLRECORD_IMPL_HPP

#include <project/ProjectAPI.hpp>
#include <project/ObjectRecord_Impl.hpp>

// TODO: Delete this include if no derived classes (and no UrlRecordType enum).
#include <project/UrlRecord.hpp>

namespace openstudio {
namespace NAMESPACE {
  class Url;
}
namespace project {

namespace detail {

  /** UrlRecord_Impl is a ObjectRecord_Impl that is the implementation class for UrlRecord.*/
  class PROJECT_API UrlRecord_Impl : public ObjectRecord_Impl {
    Q_OBJECT;
   public:
    /** @name Constructors and Destructors */
    //@{

    // TODO: May need to remove type enum if Url is a leaf of the inheritance tree.
    // TODO: Replace ProjectDatabase& database with parent Record and/or add more 
    // construtors to match public class.
    // TODO: Find-replace on 'NAMESPACE'.
    UrlRecord_Impl(const NAMESPACE::Url& url,
                   const UrlRecordType& urlRecordType,
                   ProjectDatabase& database);

    /** Constructor from query. Throws if bad query. */
    UrlRecord_Impl(const QSqlQuery& query, ProjectDatabase& database);

    virtual ~UrlRecord_Impl() {}

    //@}
    /** @name Virtual Methods */
    //@{

    /** Get the name of the database table for this record. Should be implemented by the base 
     *  class for the table. */
    virtual std::string databaseTableName() const;

    /** Returns the direct parent of this object, if it exists. */
    virtual boost::optional<ObjectRecord> parent() const;

    /** Returns objects directly owned by this Record. Children are removed when this Record 
     *  is removed. */
    virtual std::vector<ObjectRecord> children() const;

    /** Returns objects referenced, but not owned, by this Record. */
    virtual std::vector<ObjectRecord> resources() const;

    /** Returns join relationships between this object and others. Such relationships will be 
     *  removed when either record in the relationship is removed. */
    virtual std::vector<JoinRecord> joinRecords() const;

    /** Save the row that corresponds to this record in projectDatabase. */
    virtual void saveRow(ProjectDatabase& projectDatabase);

    //@}
    /** @name Getters */
    //@{

    // ADD METHODS FOR RETRIEVING PARENT, CHILD, AND RESOURCE RECORDS AS DESIRED

    // ADD METHODS FOR GETTING/SETTING SPECIFIC DATA FIELDS AS DESIRED

    NAMESPACE::Url url() const;

    //@}
   protected:
    /** Bind data member values to a query for saving. */
    virtual void bindValues(QSqlQuery& query) const;

    /** Set the last state of this object from the query (including id). */
    virtual void setLastValues(const QSqlQuery& query, ProjectDatabase& projectDatabase);

    /** Check that values (except id) are same as query. */
    virtual bool compareValues(const QSqlQuery& query) const;

    /** Save values to last state. */
    virtual void saveLastValues();

    /** Revert values back to last state. */
    virtual void revertToLastValues();

   private:
    REGISTER_LOGGER("openstudio.project.UrlRecord");

    // TODO: Delete enums if no derived classes.
    UrlRecordType m_urlRecordType;

    UrlRecordType m_lastUrlRecordType;
  };

} // detail

} // project
} // openstudio

#endif // PROJECT_URLRECORD_IMPL_HPP
