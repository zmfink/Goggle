from flask import Flask, render_template, current_app
from flask.ext.mysqldb import MySQL
import controllers
import os

app = Flask(__name__, template_folder='views')

app.register_blueprint(controllers.search)

app.secret_key = 'os.urandom(20)'
app.config['SESSION_TYPE'] = 'filesystem'

# comment this out using a WSGI like gunicorn
# if you dont, gunicorn will ignore it anyway
if __name__ == '__main__':
    # listen on external IPs
    app.run(host='0.0.0.0', port=3000, debug=True)
