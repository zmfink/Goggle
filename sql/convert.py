import xml.dom.minidom
import re

#Images Table
dom = xml.dom.minidom.parse('mining.imageUrls.xml')

statements = "INSERT INTO Images (id, picUrl)\nVALUES\n"

images = dom.getElementsByTagName("eecs485_image")
for image in images:
	article_id = image.getElementsByTagName("eecs485_article_id")[0].childNodes[0].nodeValue
	url = " "
	if image.getElementsByTagName("eecs485_png_url"):
		if image.getElementsByTagName("eecs485_png_url")[0].childNodes:
			url = image.getElementsByTagName("eecs485_png_url")[0].childNodes[0].nodeValue

	statements += "\t({0}, '{1}'),\n".format(article_id, url)

with open("load_Images.sql", "a") as output:
	output.write(statements)

# Categories Table
statements = ""
dom = xml.dom.minidom.parse('mining.category.xml')
# dom = xml.dom.minidom.parse('test.xml')


statements = "INSERT INTO Categories (id, category)\nVALUES\n"
images = dom.getElementsByTagName("eecs485_category")
for image in images:
	article_id = image.getElementsByTagName("eecs485_article_id")[0].childNodes[0].nodeValue
	article_cat = image.getElementsByTagName("eecs485_article_category")[0].childNodes[0].nodeValue
	article_cat = re.sub(r'\W+', '', article_cat)

	statements += "\t({0}, '{1}'),\n".format(article_id, article_cat)

with open("load_Categories.sql", "a") as output:
	output.write(statements)

# Infoboxes Table
dom = xml.dom.minidom.parse('mining.infobox.xml')
statements = "INSERT INTO Infoboxes (id, infobox)\nVALUES\n"
summaries = dom.getElementsByTagName("eecs485_summary")
for summary in summaries:
	infobox = ""
	article_id = summary.getElementsByTagName("eecs485_article_id")[0].childNodes[0].nodeValue
	if summary.getElementsByTagName("eecs485_article_summary"):
		if summary.getElementsByTagName("eecs485_article_summary")[0].childNodes:
			infobox = summary.getElementsByTagName("eecs485_article_summary")[0].childNodes[0].nodeValue
			infobox = infobox.replace(" ", "_")
			infobox = re.sub(r'\W+', '', infobox)

	statements += "\t({0}, '{1}'),\n".format(article_id, infobox)

with open('load_Infoboxes.sql', "a") as output:
	output.write(statements)





