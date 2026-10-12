import datetime
import streamlit as st

st.set_page_config(
    page_title="Autonomous Smart Planter IoT Simulator",
    page_icon="🌱",
    layout="wide",
)

st.title("🌱 Autonomous Smart Planter: Digital Twin & Live Simulator")
st.markdown(
    "Interactive firmware and hardware state simulator based on Arduino Uno, "
    "soil moisture thresholds, active drainage pumps, and ultrasonic/PIR deterrent systems."
)

col_ctrl, col_dash = st.columns([1, 1.2])

with col_ctrl:
  st.subheader("🎛️ Sensor Input Controls")

  soil_moisture = st.slider(
      "Soil Moisture Level (%)",
      min_value=0,
      max_value=100,
      value=35,
      help="Thresholds: Dry (<30%), Optimal (30-70%), Saturated (>70%)",
  )

  water_level = st.slider(
      "Reservoir Water Level (%)",
      min_value=0,
      max_value=100,
      value=60,
      help="Alert triggers when water drops below 20%",
  )

  motion_detected = st.toggle("PIR Motion / Intruder Detected", value=False)
  distance_cm = st.slider(
      "Ultrasonic Distance to Object (cm)",
      min_value=5,
      max_value=200,
      value=120,
      help="Triggers pest deterrent mechanism if distance < 25 cm",
  )

with col_dash:
  st.subheader("📊 System Telemetry & Actuator States")

  # Actuator & Firmware Logic
  irrigation_pump = False
  drainage_pump = False
  buzzer_active = False
  servo_active = False

  # Irrigation & Drainage
  if soil_moisture < 30:
    if water_level > 15:
      irrigation_pump = True
      moisture_status = "⚠️ Dry (Irrigation Pump ON)"
    else:
      moisture_status = "🚨 Dry, but Reservoir Empty!"
  elif soil_moisture > 75:
    drainage_pump = True
    moisture_status = "🌊 Waterlogged (Drainage Pump ON)"
  else:
    moisture_status = "✅ Optimal"

  # Pest / Intruder Deterrent Logic
  if motion_detected or distance_cm < 25:
    buzzer_active = True
    servo_active = True
    pest_status = "⚠️ Pest Detected: Alarm Active"
  else:
    pest_status = "🛡️ Perimeter Clear"

  # Visual Metric Cards
  m1, m2 = st.columns(2)
  m1.metric("Soil Condition", moisture_status)
  m2.metric("Reservoir State", f"{water_level}% Available")

  m3, m4 = st.columns(2)
  m3.metric(
      "Irrigation Pump Relay",
      "ACTIVE (Watering)" if irrigation_pump else "IDLE",
  )
  m4.metric(
      "Drainage Pump Relay", "ACTIVE (Draining)" if drainage_pump else "IDLE"
  )

  p1, p2 = st.columns(2)
  p1.metric("Pest Deterrent Flag", pest_status)
  p2.metric(
      "Servo Arm / Alarm", "TRIGGERED" if servo_active else "STANDBY"
  )

  st.divider()
  st.subheader("📜 Live Event Log")
  timestamp = datetime.datetime.now().strftime("%H:%M:%S")
  log_entries = []
  if irrigation_pump:
    log_entries.append(
        f"[{timestamp}] [ACTUATOR] Solenoid valve open. Irrigation pump"
        " activated."
    )
  if drainage_pump:
    log_entries.append(
        f"[{timestamp}] [ACTUATOR] Excessive saturation detected. Drainage"
        " active."
    )
  if buzzer_active:
    log_entries.append(
        f"[{timestamp}] [SECURITY] Motion or ultrasonic trigger at"
        f" {distance_cm}cm. Buzzer firing."
    )
  if not log_entries:
    log_entries.append(
        f"[{timestamp}] [SYSTEM] All parameters stable. Monitoring cycle"
        " normal."
    )

  st.code("\n".join(log_entries), language="text")
