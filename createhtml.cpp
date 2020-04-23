// Created by Ian Martin -- February 2020

#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// Hello all those who have wondered onto my code! I was thinking about a weird 
// side project I could do, and I decided on a note taking application of sorts.
// Basically you would take notes in a fast paced text editor, save the results
// as a .txt file, and then run this here c++ program which will create a html 
// file to run on your browser so it looks all clean and nice. 

// Of course, You need to format the .txt files correct;y. Down below in the logic
// I explain all of the different things you have to do for it to run properly.
// Feel free to explore it. anyhoo, Thanks for looking into my code I guess. <3
// --Ian Martin -- February 2020. 

vector<string>* openTextFile(string fileToOpen);

string cleanInput(string toClean);

int main()
{
    cout << "Welcome to Ian Martin\'s notes to webpage converter!" << endl;
    cout << "What file would you like to convert? --> ";
    string textFile;
    // I always use getline, screw basic cin.
    getline(cin, textFile);

    // Vector to hold the contents of the notes file.
    vector<string>* rawText;

    // Lets get the data from the file!
    rawText = openTextFile(textFile);

    if(rawText == nullptr)
    {
        cout << "Problem opening file..." << endl;
        //We got a bad file, lets exit safely.
        delete rawText;
        cout << "Exiting..." << endl;
        return 0;
    }

    //If we got here, rawText must be full of juicy data.
    //Lets start writing line by line to a vector of strings. This way we don't need to have a 
    //File stream open while we are doing processing.
    vector<string>* toFile = new vector<string>();
    //Lets have a vector to hold the table of contents links.
    vector<string>* tableOfContents = new vector<string>();
    //Lets add the beginning of the html file, since this will be the same for all files.
    toFile->push_back("<!DOCTYPE html>");
    toFile->push_back("<html>");
    toFile->push_back("<head>");
    toFile->push_back("<title>Example Note Page</title>");
    toFile->push_back("<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\">");
    toFile->push_back("<meta name=\"Author\" content=\"Ian Martin\">");
    toFile->push_back("<meta name=\"Description\" content=\"Note sheet in html form\">");
    toFile->push_back("</head>");
    //Lets also add the beginning of the body tag
    toFile->push_back("<body>");
    toFile->push_back("<div class=\"grid-container\">");
    toFile->push_back("<div class=\"item1\">");
    toFile->push_back("<!--Header!-->");
    //At this point we need to start actually proccessing data
    //The first two lines of our rawdata have to be used for title and session stuff.
    toFile->push_back("<h1>" + cleanInput(rawText->at(0)) + "</h1>");
    toFile->push_back("<h2>" + cleanInput(rawText->at(1)) + "</h2>");
    //Now we can end the title 
    toFile->push_back("</div>");
    toFile->push_back("<div class=\"item3\">");
    toFile->push_back("<!--Body!-->");

    //Now time for the body, so lets start looping through the document..
    //Ignore first two lines.
    for(int i = 2; i < rawText->size(); i++)
    {
        string line = rawText->at(i);
        //Ignore empty lines
        if(line != "")
        {
            //First we need to check for 'meta' lines
            if(line[0] == '$')
            {
                //$ signals a line needs to be added to the table of contents
                tableOfContents->push_back(cleanInput(line.substr(1)));
                //We can add a hidden div to serve as the destination.
                toFile->push_back("<p id=\"ID" + to_string(tableOfContents->size()) + "\"></p>");
            }
            else if(line[0] == '#')
            {
                //# signals a definition. Lets see its a beginning or an end
                if(line.size() <= 3) //Could be an accidental space :c
                {
                    //End of a definition div
                    toFile->push_back("</div>");
                }
                else
                {
                    //Must be the start of a definition. Lets add it to the ToC.
                    tableOfContents->push_back(cleanInput(line.substr(1)));
                    //We can add a hidden div to serve as the destination.
                    //Add the link to Wikipedia as well as the h tag.
                    toFile->push_back("<div id=\"ID" + to_string(tableOfContents->size()) + "\" class=\"defn\">");
                    toFile->push_back("<h2><a href=\"https://en.wikipedia.org/wiki/" + cleanInput(line.substr(1)) + "\" target=\"_blank\">" + cleanInput(line.substr(1)) + ":</a></h2>");
                    //Add a hr
                    toFile->push_back("<hr>");
                    //Rest of the definition will be handled by regular processing.
                }
            }
            else if(line[0] == '*')
            {
                //Lets handle lists! First we need to keep track of the depth of our list
                int depth = 0;
                while(line[0] == '*')
                {
                    //Count the depth first.
                    int temp = 0;
                    while(line[temp] == '*')
                    {
                        temp++;
                    }
                    //temp now points to the first non-* in the line.
                    if(temp > depth)
                    {
                        //If we are further than our current depth, we need to add another ul tag
                        toFile->push_back("<ul>");
                        depth = temp;
                    }
                    else if(temp < depth)
                    {
                        //If we are below the current depth, lets end the tag
                        toFile->push_back("</ul>");
                        depth = temp;
                    }
                    //Lets add the list item.
                    toFile->push_back("<li>" + cleanInput(line.substr(temp)) + "</li>");
                    //Move to the next line.
                    line = rawText->at(++i);
                }
                //Okay lets end any trailing uls.
                for(depth; depth > 0; depth--)
                {
                    toFile->push_back("</ul>");
                }
            }
            else if(line[0] == '>')
            {
                //> signifys a header, so lets do it.
                toFile->push_back("<h2>" + cleanInput(line.substr(1)) + "</h2>");
            }
            else if(line.substr(0, 3) == "<c>")
            {
                //Code!! Lets add the div that encapsulates the code
                if(line.size() < 4)
                { 
                    //Start on line 1
                    toFile->push_back("<div class=\"codes\">");
                    toFile->push_back("<pre class=\"code\">");
                }
                else
                {
                    toFile->push_back("<div class=\"codes\" style=\"counter-reset: listing " + line.substr(3) + ";\">");
                    toFile->push_back("<pre class=\"code\" style=\"counter-reset: listing " + line.substr(3) + ";\">");
                }
                //For the code itself, we can add the raw text to our file
                line = rawText->at(++i);
                while(line != "</c>" && line != "</c> ")
                {
                    toFile->push_back("<code>" + cleanInput(line) + "</code>");
                    line = rawText->at(++i);
                }
                //End the code tags
                toFile->push_back("</pre>");
                toFile->push_back("</div>");
            }
            else
            {
                //If nothing else, We are in a p tag!!
                toFile->push_back("<p>");
                //We continue until we hit a blank line.
                while(line != "" && line != " ")
                {
                    //Add the line
                    toFile->push_back(cleanInput(line));
                    line = rawText->at(++i);
                }
                //End p tag
                toFile->push_back("</p>");
            }
        }
    }

    //We have now proccessed ALL the file!! Hooray!!!!
    toFile->push_back("</div>");
    //Now we can add the ToC
    toFile->push_back("<div class=\"item2\">");
    toFile->push_back("<!--Menu!-->");
    toFile->push_back("<div class=\"navBar\">");
    toFile->push_back("<h2>Table of Contents:</h2>");
    toFile->push_back("<hr>");
    for(int j = 0; j < tableOfContents->size(); j++)
    {
        toFile->push_back("<p><a href=\"#ID" + to_string(j+1) + "\">" + tableOfContents->at(j) + "</a></p>");
        toFile->push_back("<hr>");
    }
    //End the ToC
    toFile->push_back("</div>");
    toFile->push_back("</div>");

    //Now lets end the html file C:
    toFile->push_back("<div class=\"item4\">");
    toFile->push_back("<!--Footer!-->");
    toFile->push_back("<h1><a href=\"https://github.com/Martin-Ian\" target=\"_blank\">My Github!</a></h1>");
    toFile->push_back("</div>");
    toFile->push_back("</div>");
    toFile->push_back("</body>");
    toFile->push_back("</html>");

    //All done! Lets go ahead and write it to a file!!
    ofstream outputFile;
    outputFile.open("RENAME_ME.html");
    for(int i = 0; i < toFile->size(); i++)
    {
        outputFile << toFile->at(i) << endl;
    }
    outputFile.close();

    cout << "*>*>*>*>*>*>*>*>*>*> Your html file is named RENAME_ME.html <*<*<*<*<*<*<*<*<*<*" << endl;

    //Exit safely
    delete rawText;
    delete toFile;
    delete tableOfContents;
    cout << "Exiting..." << endl;
    return 0;
}

vector<string>* openTextFile(string fileToOpen)
{
    // Lets open the stream to see if its valid.
    ifstream mystream;
    mystream.open(fileToOpen);

    if(mystream.is_open() == false)
    {
        //Bad file.. :c
        return nullptr;
    }

    // All good! We can safely proceed.
    vector<string>* rawtext = new vector<string>();
    string nextline;
    while(mystream.good())
    {
        //Grab the line of text, and slap that bad boy into our vector
        getline(mystream, nextline);
        rawtext->push_back(nextline);
    }

    //Make sure to close the stream
    mystream.close();

    return rawtext;
}

//This function will go through a string and change < and > to &lt and &gt
string cleanInput(string toClean)
{
    string temp;
    for(int i = 0; i < toClean.size(); i++)
    {
        switch(toClean[i])
        {
            case '<':
                temp.append("&lt");
                break;
            case '>':
                temp.append("&gt");
                break;
            default:
                temp.push_back(toClean[i]);
                break;
        }
    }
    return temp;
}