from flask import Blueprint, jsonify, send_file
from .gemini import generate_compliment
from gtts import gTTS
import io

def init_routes(app):
    @app.route("/")
    def home():
        return "Hello"

    @app.route("/get_compliment", methods=["GET"])
    def get_compliment():
        compliment = generate_compliment()
        return jsonify({
            "text": compliment
        })
    
    @app.route("/get_audio", methods=["GET"])
    def get_audio():
        compliment = generate_compliment()
        tts = gTTS(text = compliment, lang="en")

        audio_fp = io.BytesIO()
        tts.write_to_fp(audio_fp)
        audio_fp.seek(0)

        return send_file(
            audio_fp,
            mimetype="audio/mpeg",
            as_attachment=False,
            download_name="compliment.mp3"
        )