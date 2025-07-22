from celery_app import app
from itertools import islice
import json

@app.task
def readChunkAndSumDuration(filePath, start, end):
    total_duration = 0
    skipped = 0
    
    def parse_duration(duration):
        """Parses a duration string like '3:45' or '45' into seconds."""
        try:
            parts = duration.split(":")
            if len(parts) == 2:
                return int(parts[0]) * 60 + int(parts[1])
            elif len(parts) == 1:
                return int(parts[0])
        except ValueError:
            return 0
        return 0
    
    with open(filePath, 'r', encoding='utf-8') as f:
        for line in islice(f, start, end):
            try:
                song = json.loads(line)
                duration_str = song.get('Length', '0:00')
                total_duration += parse_duration(duration_str)
            except (json.JSONDecodeError, TypeError):
                skipped += 1
                continue

    print(f"Chunk {start}-{end}: total duration = {total_duration} seconds (skipped {skipped} lines)")
    return total_duration


@app.task
def aggregateResults(results):
    return sum(results)

@app.task
def aggregateResultsAndCalculateAverage(results, totalLines):
    return sum(results) / totalLines

@app.task
def readChunkAndMapArtistSongPopularity(filePath, start, end):
    skipped = 0
    artistStats = {}
    with open(filePath, 'r', encoding='utf-8') as f:
        for line in islice(f, start, end):
            try:
                song = json.loads(line)
                artists = song.get("Artist(s)", "").strip()
                popularity = int(song.get("Popularity") or 0)
                if(artists):
                    artistsList = artists.split(",")
                    if len(artistsList) == 1:  # Ignore feats
                        artist = artistsList[0]
                        count, totalPopularity = artistStats.get(artist, (0, 0))
                        artistStats[artist] = (count + 1, totalPopularity + popularity)
            except (json.JSONDecodeError, TypeError):
                skipped += 1
                continue

    print(f"Skipped {skipped} lines")
    return artistStats

@app.task
def getTop100ProductiveArtists(artistSongDicts, topN):
    finalStats = {}

    for artistSongDict in artistSongDicts:
        for artist, (count, popularity) in artistSongDict.items():
            prevCount, prevPopularity = finalStats.get(artist, (0, 0))
            finalStats[artist] = (prevCount + count, prevPopularity + popularity)
    
    n = min(topN, len(finalStats))
    topNArtists = sorted(finalStats.items(), key=lambda x: x[1][0], reverse=True)[:topN]

    # Calculate average popularity per artist
    return {
        artist: totalPopularity / count for artist, (count, totalPopularity) in topNArtists
    }

@app.task
def readChunkAndMapExplicityPopularity(filePath, start, end):
    skipped = 0
    explicityMap = {}
    with open(filePath, 'r', encoding='utf-8') as f:
        for line in islice(f, start, end):
            try:
                song = json.loads(line)
                explicity = song.get("Explicit", "").strip().lower()
                popularity = int(song.get("Popularity") or 0)
                count, totalPopularity = explicityMap.get(explicity, (0, 0))
                explicityMap[explicity] = (count + 1, totalPopularity + popularity)
            except (json.JSONDecodeError, TypeError):
                skipped += 1
                continue

    print(f"Skipped {skipped} lines")
    return explicityMap

@app.task
def aggragateExplicityAndCalculateAverage(explicityMaps):
    aggregateExplicityMap = {}

    for explicityMap in explicityMaps:
        for explicity, (count, popularity) in explicityMap.items():
            prevCount, prevPopularity = aggregateExplicityMap.get(explicity, (0, 0))
            aggregateExplicityMap[explicity] = (prevCount + count, prevPopularity + popularity)

    # Calculate average popularity per explicity
    return {
        explicity: totalPopularity / count for explicity, (count, totalPopularity) in aggregateExplicityMap.items()
    }

@app.task
def readChunkAndMapYearDanceability(filePath, start, end):
    skipped = 0
    danceByYearMap = {
        "before-2001": (0, 0),   # (count, total_danceability)
        "2001-2012": (0, 0),
        "after-2012": (0, 0)
    }

    with open(filePath, 'r', encoding='utf-8') as f:

        def extractYear(date_string):
            try:
                return int(date_string.split("-")[0])
            except (ValueError, AttributeError):
                return 0

        for line in islice(f, start, end):
            try:
                song = json.loads(line)
                date = song.get("Release Date", "")
                if(not date):
                    skipped += 1
                    continue
                year = extractYear(date)
                danceability = int(song.get("Danceability") or 0)

                if year <= 2001:
                    count, total = danceByYearMap["before-2001"]
                    danceByYearMap["before-2001"] = (count + 1, total + danceability)
                elif year <= 2012:
                    count, total = danceByYearMap["2001-2012"]
                    danceByYearMap["2001-2012"] = (count + 1, total + danceability)
                else:
                    count, total = danceByYearMap["after-2012"]
                    danceByYearMap["after-2012"] = (count + 1, total + danceability)

            except (json.JSONDecodeError, TypeError, ValueError):
                skipped += 1
                continue

    print(f"Skipped {skipped} lines")
    return danceByYearMap

@app.task
def aggragateDanceabilityAndCalculateAverage(danceByYearMaps):
    aggreageteDanceBYYearMap = {}

    for danceByYearMap in danceByYearMaps:
        for yearGroup, (count, danceability) in danceByYearMap.items():
            prevCount, prevDanceability = aggreageteDanceBYYearMap.get(yearGroup, (0, 0))
            aggreageteDanceBYYearMap[yearGroup] = (prevCount + count, prevDanceability + danceability)

    # Calculate average popularity per year group
    return {
        yearGroup: totalDanceability / count for yearGroup, (count, totalDanceability) in aggreageteDanceBYYearMap.items()
    }


    