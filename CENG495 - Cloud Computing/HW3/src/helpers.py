from tasks import *
from celery import chord

# Returns list of chunk boundaries e.g: [(0,1), (1,2), (2,3)]
def calculateChunkBoundaries(totalLines, numberOfCpus, minChunkSize=2000, maxChunkSize=100000):
    chunks = []
    base = totalLines // numberOfCpus

    if base <= minChunkSize:
        start = 0
        while start < totalLines:
            end = min(start + minChunkSize, totalLines)
            chunks.append((start, end))
            start = end

    elif base <= maxChunkSize:
        remainder = totalLines % numberOfCpus
        start = 0
        for i in range(numberOfCpus):
            end = start + base + (1 if i < remainder else 0)
            if start >= totalLines:
                break
            chunks.append((start, min(end, totalLines)))
            start = end

    else:
        start = 0
        while start < totalLines:
            end = min(start + maxChunkSize, totalLines)
            chunks.append((start, end))
            start = end

    return chunks

# Calculate total duration of the songs
def calculateTotalDurations(filePath, chunks):
    return chord((readChunkAndSumDuration.s(filePath, start, end) for start, end in chunks), aggregateResults.s())()

# Calculate average duration of the songs:
def calculateAverageDurations(filePath, chunks, totalLines):
    return chord((readChunkAndSumDuration.s(filePath, start, end) for start, end in chunks), aggregateResultsAndCalculateAverage.s(totalLines))()

# Calculate average popularity of songs of artists who are in the 100 artists with the highest number of songs
def calculateAvgPopularityOfTop100(filePath, chunks, topN = 100):
    return chord((readChunkAndMapArtistSongPopularity.s(filePath, start, end) for start, end in chunks), getTop100ProductiveArtists.s(topN))()

# Calculate average popularity of songs of artists who are in the 100 artists with the highest number of songs
def calculateAvgPopularityOfExplicity(filePath, chunks):
    return chord((readChunkAndMapExplicityPopularity.s(filePath, start, end) for start, end in chunks), aggragateExplicityAndCalculateAverage.s())()

# Calculate average danceability of song grouped by years
def calculateAvgDanceabilityPerYear(filePath, chunks):
    return chord((readChunkAndMapYearDanceability.s(filePath, start, end) for start, end in chunks), aggragateDanceabilityAndCalculateAverage.s())()