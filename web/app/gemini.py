import google.generativeai as genai
import os

GOOGLE_API_KEY = os.getenv("GOOGLE_API_KEY")

genai.configure(api_key=GOOGLE_API_KEY)
model = genai.GenerativeModel('gemini-2.0-flash')

def generate_compliment():
    prompt = "Give me a short, one-sentence, kind compliment to make someone smile."

    try:
        response = model.generate_content(prompt)
        return response.text.strip()
    
    except Exception as e:
        print("Gemini error:", e)
        return "Fallback"