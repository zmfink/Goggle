//OLD
//word  -> idf -> total #occurrences -> Doc_id -> number of occurrences in Doc_id -> Doc_id's normalization factor (before sqrt)


//NEW
//word docfreq docid:tf-idf

#include "Index_server.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <unordered_set>

#include "mongoose.h"

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ostream;
using std::ostringstream;
using std::string;
using std::vector;

using namespace std;

namespace {
    int handle_request(mg_connection *);
    int get_param(const mg_request_info *, const char *, string&);
    string get_param(const mg_request_info *, const char *);
    string to_json(const vector<Query_hit>&);

    ostream& operator<< (ostream&, const Query_hit&);
}

pthread_mutex_t mutex1;

vector<string> stop_words;

// Runs the index server on the supplied port number.
void Index_server::run(int port)
{
    // List of options. Last element must be NULL
    ostringstream port_os;
    port_os << port;
    string ps = port_os.str();
    const char *options[] = {"listening_ports",ps.c_str(),0};

    // Prepare callback structure. We have only one callback, the rest are NULL.
    mg_callbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.begin_request = handle_request;

    // Initialize the global mutex lock that effectively makes this server
    // single-threaded.
    pthread_mutex_init(&mutex1, 0);

    // Start the web server
    mg_context *ctx = mg_start(&callbacks, this, options);
    if (!ctx) {
        cerr << "Error starting server." << endl;
        return;
    }

    pthread_exit(0);
}

struct weights{
	int id;
	double tf_idf;
};

struct entries{
	int docfreq;
	vector<weights> scores;
};


unordered_map< string, entries > inverted_index;
int totaldocs;

// Load index data from the file of the given name.
void Index_server::init(ifstream& infile)
{
	unordered_set<int> allDocs;
    stringstream ss;
    string line;

    string word;
    long long int id;
	double tf_idf;
	int docfreq;
	string both;

    //read terms into the map
    while(getline(infile, line)){
        stringstream ss;
        ss.str(line);
		ss >> word >> docfreq;
		
		entries entry;
		entry.docfreq = docfreq;
		
		while(ss >> both){
			int pos = both.find(":");
			string id_string = both.substr(0, pos);
			string tf_idf_string = both.substr(pos + 1);
			id = stoll(id_string, nullptr);
			tf_idf = stod(tf_idf_string, nullptr);
			
			weights weight;
			weight.id = id;
			weight.tf_idf = tf_idf;
			
			entry.scores.push_back(weight);
			allDocs.insert(id);
		}
		
        inverted_index[word] = entry;
    }
    infile.close();
	totaldocs = allDocs.size();
	
    ifstream stopfile;
    stopfile.open("stop.txt");
    while(stopfile >> word){
        stop_words.push_back(word);
    }
    stopfile.close();

    //TESTING DELETE LATER
    // string query = "19";
    // vector<Query_hit> hits;
    // process_query(query, hits);

    return;
}

// Search the index for documents matching the query. The results are to be
// placed in the supplied "hits" vector, which is guaranteed to be empty when
// this method is called.
void Index_server::process_query(const string& query, vector<Query_hit>& hits)
{
	cout << "Processing query '" << query << "'" << endl;
    stringstream ss;
    ss.str(query);
    vector<string> queries;
    string word;
    while(ss >> word){
        //strip non alphanumeric letters
        for(auto it = word.begin(); it != word.end(); it++){
            if(!isalnum(word.at(it - word.begin()))){
                word.erase(it);
                it--;
             }
         }
         //turn to lowercase
         transform(word.begin(), word.end(), word.begin(), ::tolower);

         //check it's not a stop word
         if(find(stop_words.begin(), stop_words.end(), word) == stop_words.end() && (word != "")){
            auto it = inverted_index.find(word);
            //exit if word is not in inverted_index
            if(it == inverted_index.end())
                return;

            //put terms in queries.
            bool found = false;
            for(int i = 0; i < queries.size(); ++i){
                if(queries[i] == word){
                    found = true;
                }
            }
            if(found == false){
                queries.push_back(word);
            }
         }
     }
     //calculate norm factor for caption
     double norm_factor_caption = 0;
     for(int i = 0; i < queries.size(); ++i){
		double idf = log10(totaldocs / inverted_index[word].docfreq);
        norm_factor_caption += pow(idf, 2);
     }
     norm_factor_caption = sqrt(norm_factor_caption);

     //fill hits with docs that contain first word
     if(!queries.empty()){

        //calculate W(i,j) for first query
        string word = queries[0];
        double query_weight;
        if(norm_factor_caption == 0)
            query_weight = 0;
        else
            query_weight = log10(totaldocs / inverted_index[word].docfreq) / norm_factor_caption;
        //loop through docs that have the word
        for(int i = 0; i < inverted_index[word].scores.size(); ++i){

            //calculate W(i,k)
            entries termEntry = inverted_index[word];

            //push_back hit to hits vector
			const char* i_str = to_string(termEntry.scores[i].id).c_str();
			char* id_string = new char[50];
			strcpy(id_string, i_str);
            Query_hit new_query_hit(id_string, termEntry.scores[i].tf_idf * query_weight);
            hits.push_back(new_query_hit);
        }
     }

    //update the scores of each doc for every word
    for(int i = 1; i < queries.size(); ++i){
		string word = queries[i];
        //look for a doc in index.docs with the same id as the hit
        for(int j = 0; j < hits.size(); ++j){
            const char* docid = hits[j].id;
            bool found = false;
            auto it = inverted_index[word].scores;
            int doc_loc = 0;

            //loop through scores
            for(int k = 0; k < it.size(); ++k){
				const char* i_str = to_string(it[k].id).c_str();
				char* id_string = new char[50];
				strcpy(id_string, i_str);
                if(strcmp(docid, id_string) == 0){
                    found = true;
                    doc_loc = k;
                }
            }
            //if there's no match in docs, erase it from hits
            if(found == false){
                hits.erase(hits.begin() + j);
                //adjust for the shrunken vector
                j -= 1;
            }

            else{
                //calculate W(i,k)
                entries termEntry = inverted_index[word];
                
                //calculate W(i,j) for the i'th query
                double query_weight;
                if(norm_factor_caption == 0)
                    query_weight = 0;
                else
				    query_weight = log10(totaldocs / termEntry.docfreq) / norm_factor_caption;

                //update score of hit
                hits[j].score += (query_weight * termEntry.scores[doc_loc].tf_idf);
            }
        }
    }
    // if(!hits.empty())
    //     cout << hits.size() << endl;
    //     cout << hits[0] << endl;
    //     cout << hits[1] << endl;
    return;
}

namespace {
    int handle_request(mg_connection *conn)
    {
        const mg_request_info *request_info = mg_get_request_info(conn);

        if (!strcmp(request_info->request_method, "GET") && request_info->query_string) {
            // Make the processing of each server request mutually exclusive with
            // processing of other requests.

            // Retrieve the request form data here and use it to call search(). Then
            // pass the result of search() to to_json()... then pass the resulting string
            // to mg_printf.
            string query;
            if (get_param(request_info, "q", query) == -1) {
                // If the request doesn't have the "q" field, this is not an index
                // query, so ignore it.
                return 1;
            }

            vector<Query_hit> hits;
            Index_server *server = static_cast<Index_server *>(request_info->user_data);

            pthread_mutex_lock(&mutex1);
            server->process_query(query, hits);
            pthread_mutex_unlock(&mutex1);

            string response_data = to_json(hits);
            int response_size = response_data.length();

            // Send HTTP reply to the client.
            mg_printf(conn,
                      "HTTP/1.1 200 OK\r\n"
                      "Content-Type: application/json\r\n"
                      "Content-Length: %d\r\n"
                      "\r\n"
                      "%s", response_size, response_data.c_str());
        }

        // Returning non-zero tells mongoose that our function has replied to
        // the client, and mongoose should not send client any more data.
        return 1;
    }

    int get_param(const mg_request_info *request_info, const char *name, string& param)
    {
        const char *get_params = request_info->query_string;
        size_t params_size = strlen(get_params);

        // On the off chance that operator new isn't thread-safe.
        pthread_mutex_lock(&mutex1);
        char *param_buf = new char[params_size + 1];
        pthread_mutex_unlock(&mutex1);

        param_buf[params_size] = '\0';
        int param_length = mg_get_var(get_params, params_size, name, param_buf, params_size);
        if (param_length < 0) {
            return param_length;
        }

        // Probably not necessary, just a precaution.
        param = param_buf;
        delete[] param_buf;

        return 0;
    }

    // Converts the supplied query hit list into a JSON string.
    string to_json(const vector<Query_hit>& hits)
    {
        ostringstream os;
        os << "{\"hits\":[";
        vector<Query_hit>::const_iterator viter;
        for (viter = hits.begin(); viter != hits.end(); ++viter) {
            if (viter != hits.begin()) {
                os << ",";
            }

            os << *viter;
        }
        os << "]}";

        return os.str();
    }

    // Outputs the computed information for a query hit in a JSON format.
    ostream& operator<< (ostream& os, const Query_hit& hit)
    {
        os << "{" << "\"id\":\"";
        int id_size = strlen(hit.id);
        for (int i = 0; i < id_size; i++) {
            if (hit.id[i] == '"') {
                os << "\\";
            }
            os << hit.id[i];
        }
        return os << "\"," << "\"score\":" << hit.score << "}";
    }
}
