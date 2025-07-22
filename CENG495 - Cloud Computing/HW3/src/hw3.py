from tasks import *
from helpers import *
from celery import chord
import json
import os

filePath = '900k Definitive Spotify Dataset.json'

# Count lines
with open(filePath, 'r', encoding='utf-8') as f:
    totalLines = sum(1 for _ in f)

# Get number of logical CPUs
numberOfCpus = os.cpu_count() or 8

# Compute chunk sizes
chunks = calculateChunkBoundaries(totalLines, numberOfCpus)

# Q1
total = calculateTotalDurations(filePath, chunks)

# Q2
average = calculateAverageDurations(filePath, chunks, totalLines)

# Q3
averagePopularity = calculateAvgPopularityOfTop100(filePath, chunks)

# Q4
explicityPopularity = calculateAvgPopularityOfExplicity(filePath, chunks)

# Q5
dancebyyear = calculateAvgDanceabilityPerYear(filePath, chunks)

totalResult = {
    "total": total.get(),
    "average": average.get(),
    "explicit-popularity": explicityPopularity.get(),
    "artist-popularity": averagePopularity.get(),
    "dancebyyear": dancebyyear.get()
}

with open('results.json', 'w') as f:
    json.dump(totalResult, f, indent=2)
