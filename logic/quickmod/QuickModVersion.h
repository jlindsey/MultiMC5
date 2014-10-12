/* Copyright 2013 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <QString>
#include <QMap>
#include <QCryptographicHash>
#include <QUrl>
#include <QStringList>

#include "logic/BaseVersion.h"
#include "logic/net/NetJob.h"
#include "logic/net/CacheDownload.h"
#include "logic/tasks/Task.h"
#include "logic/quickmod/QuickModMetadata.h"
#include "QuickModDownload.h"
#include "QuickModVersionRef.h"
#include <logic/settings/SettingsObject.h>
#include "modutils.h"
#include "pathutils.h"
#include "MultiMC.h"

class OneSixInstance;
class BaseInstance;
typedef std::shared_ptr<BaseInstance> InstancePtr;

class QuickModVersion;
typedef std::shared_ptr<QuickModVersion> QuickModVersionPtr;
Q_DECLARE_METATYPE(QuickModVersionPtr)

class QuickModVersion : public BaseVersion
{
public: /* types */
	enum InstallType
	{
		ForgeMod,
		ForgeCoreMod,
		LiteLoaderMod,
		Extract,
		ConfigPack,
		Group
	};

	struct Library
	{
		Library()
		{
		}
		Library(const QString &name, const QUrl &repo) : name(name), repo(repo)
		{
		}
		QJsonObject toJson() const;
		QString name;
		QUrl repo;
	};

public: /* construction */
	QuickModVersion(QuickModMetadataPtr mod, bool valid = true) : mod(mod)
	{
	}

	static QList<QuickModVersionPtr> parse(const QJsonObject &object, QuickModMetadataPtr mod);
	void parse(const QJsonObject &object);
	QJsonObject toJson() const;

public: /* methods */
	QString descriptor() const override
	{
		return m_version.toString();
	}

	QString name() const override
	{
		return versionName;
	}

	QString typeString() const
	{
		return type;
	}

	QuickModVersionRef version() const
	{
		return QuickModVersionRef(mod->uid(), m_version);
	}

	/// get the file name for the mod file.
	QString fileName() const;

	
	MetaEntryPtr cacheEntry() const;

	/// get the local cache storage path for the mod
	QString storagePath() const;

	/// get the instance deploy path for the mod
	QString instancePath() const;

	/// Get a download to use.
	QuickModDownload highestPriorityDownload(const QuickModDownload::DownloadType type = QuickModDownload::Invalid);

	/// Does this actually need to be deployed into the instance?
	bool needsDeploy() const;

	/// install this version of package into the specified instance
	void installInto(std::shared_ptr<OneSixInstance> m_instance);

private: /* methods */
	void installLibrariesInto(std::shared_ptr<OneSixInstance> m_instance);

public: /* data */
	/// quickmod this is associated with
	QuickModMetadataPtr mod;

	/// fluff display name for the version ('name' in the spec)
	QString versionName;

	/// actual version id ('version' in the spec)
	Util::Version m_version;

	/// a thing. idk what it is
	QString versionString;

	/// A type of the version, for example Release, Dev, Alpha or Beta.
	QString type;

	// FIXME: versions of minecraft? how does that work now? TODO!
	QStringList mcVersions;

	QMap<QuickModRef, QPair<QuickModVersionRef, bool>> dependencies;
	QMap<QuickModRef, QuickModVersionRef> recommendations;
	QMap<QuickModRef, QuickModVersionRef> suggestions;
	QMap<QuickModRef, QuickModVersionRef> conflicts;
	QMap<QuickModRef, QuickModVersionRef> provides;

	// FIXME: move these to some 'File' object
	/// SHA1 checksum of the downloaded file.
	QString sha1;

	/// how to install the mod file?
	InstallType installType;

	/// list of libraries to add for this mod
	QList<Library> libraries;

	/// list of download locations for the file.
	QList<QuickModDownload> downloads;
};