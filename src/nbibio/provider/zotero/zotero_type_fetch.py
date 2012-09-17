#!/usr/bin/env kross

import json
import urllib2
import httplib2

import pprint


def fetchCreators(zoterotype):
    h = httplib2.Http(".cache")
    resp, content = h.request('https://api.zotero.org/itemTypeCreatorTypes?itemType=' + zoterotype )

    decoded = json.loads(content)

    keylist = ''
    for entry in decoded:
        for key, value in entry.iteritems():
            if key == 'creatorType':
                transformedCreator = value
                if valueTransform.has_key(value):
                    transformedCreator = valueTransform[value]
                    
                keylist += value + ':' + transformedCreator + ';'

    return keylist[:-1]

def fetchNewEntry(zoterotype):
    h = httplib2.Http(".cache")
    resp, content = h.request('https://api.zotero.org/items/new?itemType=' + zoterotype )

    decoded = json.loads(content)

    print '\n[' + zoterotype + ']'
    for key, value in decoded.iteritems():
        if key == 'creators':
            print key + ' = ' + fetchCreators(zoterotype)
        else:
            value = key
            if valueTransform.has_key(key):
                value = valueTransform[key]
            print key + ' = ' + value

#------------------------------------------------------------------------------------------------
#
#------------------------------------------------------------------------------------------------

valueTransform = dict(
                        callNumber = 'lccn',
                        rights = 'copyright',
                        tags = 'keywords',
                        seriesTitle = 'series',
                        blogTitle = 'booktitle',
                        abstractNote = 'abstract',
                        dictionaryTitle = 'booktitle',
                        encyclopediaTitle = 'booktitle',
                        publicationTitle = 'journal',
                        forumTitle = 'booktitle',
                        proceedingsTitle = 'booktitle',
                        websiteType = 'type',
                        seriesNumber = 'number',
                        distributor = 'publisher',
                        manuscriptType = 'type',
                        episodeNumber = 'number',
                        presentationType = 'type',
                        reportType = 'type',
                        reportNumber = 'number',
                        thesisType = 'type',
                        university = 'school',
                        websiteTitle = 'booktitle',
                        programTitle = 'booktitle',
                        place = 'address',
                        billNumber = 'number',
                        docketNumber = 'number',
                        caseName = 'title',
                        documentNumber = 'number',
                        issue = 'number',
                        patentNumber = 'number',
                        publicLawNumber = 'number',
                        letterType = 'type',
                        serie = 'series',
                        label = 'publisher',
                        section = 'chapter',
                        nameOfAct = 'title',
                        postType = 'type',
                        codePages = 'pages',
                        court = 'organization',
                        issuingAuthority = 'organization',
                        legislativeBody = 'organization',
                        session = 'event',
                        dateDecided = 'date',
                        dateEnacted = 'date',
                        firstPage = 'pages',
                        issueDate = 'date',
                        meetingName = 'event',
                        conferenceName = 'event',
                        mapType = 'type',
                        artist = 'author',
                        performer = 'author',
                        sponsor = 'author',
                        programmer = 'author',
                        director = 'author',
                        interviewee = 'author',
                        cartographer = 'author',
                        inventor = 'author',
                        podcaster = 'author',
                        presenter = 'author',
                        composer = 'editor'
                        )

h = httplib2.Http(".cache")
resp, content = h.request('https://api.zotero.org/itemTypes' )

decoded = json.loads(content)

for entry in decoded:
    for key, value in entry.iteritems():
        if key == 'itemType':
            fetchNewEntry( value)