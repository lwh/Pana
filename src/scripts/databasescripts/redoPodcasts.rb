#!/usr/bin/env ruby

query = `dcop pana collection query "SELECT url FROM podcastchannels;"`
print "Grabbing podcast feeds from database...\n"

podcasts = query.split("\n")

print "Delete all the podcasts from the playlist browser.  Once done, press something. Don't screw this up."
message = gets()
print "Sure you have removed them?"
message = gets()

podcasts.each do |channel|
    print "Adding podcast: #{channel}\n"
    system("dcop", "pana", "playlistbrowser", "addPodcast", channel)
end
print "Done.\n"
