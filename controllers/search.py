from flask import *
import MySQLdb
import os
import requests
import datetime
from werkzeug import secure_filename
from flask import Flask, request, redirect, url_for
import operator
import json, ast

search = Blueprint('search', __name__, template_folder='views')

@search.route('/iqv5o4/pa6/search')
def search_route():
	if not(request.args.has_key("w")):
		return render_template("search.html", search = False, title = "Search")
	else:
		w_val = request.args.get("w")
		w_val = float(w_val)

	if not(request.args.has_key("query")):
		return render_template("search.html", search = False, title = "Search")

	_query = request.args.get("query")

	result = requests.get('http://eecs485-09.eecs.umich.edu:5892/search?q=' + _query)
	#result = requests.get('http://0.0.0.0:5892/search?q=' + _query)

	r = json.loads(result.text)
	r = r['hits']

	searchList = list()
	for row in r:
		search = { 'score': row['score'], 'id': str(row['id'])}
		searchList.append(search)


	prDict = {}

	pr_file = open("proutput/pgrkoutput", "r")
	for line in pr_file:
		key, value = line.split(", ")
		prDict[key.strip()] = float(value.strip())

	for page in searchList:
		#Score(q, d) = (w * PageRank(d)) + ((1-w) * tfIdf(q, d))
		temp = page['id']
		if temp in prDict:
			prScore = prDict[temp]
		else:
			prScore = 0
		page['score'] = (w_val * prScore) + ((1 - w_val) * page['score'])


	sorted_result = sorted(searchList, key=lambda p: p['score'], reverse = True)
	sorted_result = sorted_result[:10]
	
	conn = MySQLdb.connect("localhost", "group92", "jonsnow", "group92")
	#conn = MySQLdb.connect(host = "localhost", user = "localhost", db =  "group92")

	cursor = conn.cursor()
	queryList = list()

	for entry in sorted_result:
		cursor.execute("SELECT picurl FROM Images WHERE id={0}".format(entry['id']))
		picurl = cursor.fetchone()
		if picurl is None:
			picurl =""
		else:
			picurl = picurl[0]
		cursor.execute("SELECT category FROM Categories WHERE id={0}".format(entry['id']))
		category = cursor.fetchone()
		if category is None:
			category = ""
		else:
			category = category[0]
		cursor.execute("SELECT infobox FROM Infoboxes WHERE id={0}".format(entry['id']))
		infobox = cursor.fetchone()
		if infobox is None:
			infobox = ""
		else:
			infobox = infobox[0]
		data = {
			"id": entry['id'],
			"picurl": picurl,
			"category": category,
			"infobox": infobox
		}
		queryList.append(data)

	hits = len(queryList)

	return render_template("search.html", queryList = queryList, search = True, title = "Search", hits = hits, w_val = w_val)