/*
 * Copyright (c) 2025 Amir Czwink (amir130@hotmail.de)
 *
 * This file is part of FolderCleaner.
 *
 * FolderCleaner is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FolderCleaner is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FolderCleaner.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <StdXX.hpp>
using namespace StdXX;
using namespace StdXX::FileSystem;

String g_contentTypesToDelete[] = {
		FileFormats::FileMetaData::appleDesktopServicesStore,
	FileFormats::FileMetaData::appleDouble,
};

static bool IsInDeleteList(const FileFormats::FileFormat& format)
{
	for(const auto& contentType : g_contentTypesToDelete)
	{
		if(format.GetMediaTypes().Contains(contentType))
			return true;
	}
	return false;
}

static void CleanFileIfNeeded(const Path& filePath, bool deleteFromFS)
{
	const auto& registry = FileFormats::FormatRegistry::Instance();

	UniquePointer<FileInputStream> fileInputStream = new FileInputStream(filePath);
	auto format = registry.ProbeFormat(*fileInputStream);
	if((format != nullptr) && IsInDeleteList(*format))
	{
		File file(filePath);
		stdOut << u8"Removing file '" << filePath << u8"' (" << String::FormatBinaryPrefixed(file.Size()) << u8") with format: " << format->GetName() << endl;
		if(deleteFromFS)
			file.DeleteFile();
	}
}

static void CleanDirectory(const Path& dirPath, const bool deleteFromFS)
{
	File dir(dirPath);

	for(const auto& entry : dir)
	{
		auto childPath = dirPath / entry.name;
		switch(entry.type)
		{
			case FileType::Directory:
				CleanDirectory(childPath, deleteFromFS);
				break;
			case FileType::File:
				CleanFileIfNeeded(childPath, deleteFromFS);
				break;
			default:
				NOT_IMPLEMENTED_ERROR; //TODO: implement me
		}
	}

	bool hasChildren = false;
	for(const auto& entry : dir)
	{
		hasChildren = true;
		break;
	}
	if(!hasChildren)
	{
		stdOut << u8"Removing empty directory: " << dirPath << endl;
		if(deleteFromFS)
			dir.RemoveDirectory();
	}
}

int32 Main(const String &programName, const FixedArray<String> &args)
{
	CommandLine::Parser parser(programName);

	parser.AddHelpOption();

	CommandLine::PathArgument sourcePathArg(u8"source-path", u8"Path to the source folder");
	parser.AddPositionalArgument(sourcePathArg);

	CommandLine::Option dryRun(u8'd', u8"dry-run", u8"Report what would be done without doing it.");
	parser.AddOption(dryRun);

	if(!parser.Parse(args))
	{
		parser.PrintHelp();
		return EXIT_FAILURE;
	}

	const CommandLine::MatchResult& result = parser.ParseResult();

	CleanDirectory(sourcePathArg.Value(result), !result.IsActivated(dryRun));

	stdOut << u8"Done." << endl;

	return EXIT_SUCCESS;
}