<div style="font-family:sans-serif;max-width:900px;margin:auto;color:#333;line-height:1.7">

<div style="background:linear-gradient(135deg,#2a9d8f,#264653);padding:32px;border-radius:14px;color:white;text-align:center;margin-bottom:30px">
<h1 style="margin:0">WKnob Wireless Knob</h1>
  <img width="993" height="691" alt="image" src="https://github.com/user-attachments/assets/d59263a9-466b-446b-97ee-9a71211f7ad1" />

<p style="opacity:0.9;margin-top:4px">BLE · HomeAssistant · BTHome · Encoder · Accelerometer Interaction</p>
</div>

<hr style="border:none;border-top:1px solid #ccc;margin:30px 0" />

<div style="background:#f8f9fa;padding:20px;border-radius:10px;border-left:5px solid #2a9d8f">
<h3 style="color:#2a9d8f;margin-top:0">Introduction</h3>
<p>I’ve long wanted to build a <strong>wireless knob controller</strong>, but off-the-shelf wireless buttons are already very cheap (around ¥10). DIYing a fully enclosed device didn’t seem cost-effective, so the idea was put on hold for a while.</p>
<p>Later, I noticed that <strong>wireless knobs with encoders are actually quite rare</strong>.</p>
<p>That gave me a new idea:<br />
• Add an <strong>encoder knob</strong>,<br />
• Include an <strong>accelerometer</strong>,<br />
• Unlock more <strong>interesting interactive features</strong>.</p>
<p>And so this project was born.</p>
</div>

<hr style="border:none;border-top:1px solid #ccc;margin:30px 0" />

<div style="background:#f8f9fa;padding:16px 18px;border-radius:10px;border-left:5px solid #2a9d8f">
<h3 style="color:#2a9d8f;margin:0 0 10px 0">Project Demo (click to view)</h3>
<div style="text-align:center">
<a href="https://www.bilibili.com/video/BV1ejPszpENa/" target="_blank">
<img width="828" height="486" alt="image" src="https://github.com/user-attachments/assets/4077f421-f89e-4679-a1d7-e0fd7f005ad5" />
</a>
</div>
</div>


<hr style="border:none;border-top:1px solid #ccc;margin:30px 0" />

<div style="border-left:4px solid #2a9d8f;padding:12px 16px;font-family:system-ui,sans-serif;max-width:800px">
<h3 style="margin:0 0 10px 0;color:#2a9d8f">Features Overview</h3>
<table style="width:100%;border-collapse:collapse;font-size:14px">
<thead>
<tr style="background:#f5f5f5">
<th style="padding:8px;border:1px solid #ddd;width:28%">Interaction</th>
<th style="padding:8px;border:1px solid #ddd">Function Description</th>
</tr>
</thead>
<tbody>
<tr>
<td style="padding:8px;border:1px solid #ddd">🔄 Encoder Knob</td>
<td style="padding:8px;border:1px solid #ddd">Rotation changes encoder value, range <b>-127 ~ +127</b>, supports cyclic counting</td>
</tr>
<tr>
<td style="padding:8px;border:1px solid #ddd">🔘 Single Click</td>
<td style="padding:8px;border:1px solid #ddd">Triggers <b>single-click event</b></td>
</tr>
<tr>
<td style="padding:8px;border:1px solid #ddd">🔘 Double Click</td>
<td style="padding:8px;border:1px solid #ddd">Triggers <b>double-click event</b></td>
</tr>
<tr>
<td style="padding:8px;border:1px solid #ddd">🔘 Triple Click</td>
<td style="padding:8px;border:1px solid #ddd">Triggers <b>triple-click event</b></td>
</tr>
<tr>
<td style="padding:8px;border:1px solid #ddd">⏳ Single Long Press</td>
<td style="padding:8px;border:1px solid #ddd">Triggers <b>single long-press event</b></td>
</tr>
<tr>
<td style="padding:8px;border:1px solid #ddd">⏳ Double Long Press</td>
<td style="padding:8px;border:1px solid #ddd">Triggers <b>double long-press event</b></td>
</tr>
<tr>
<td style="padding:8px;border:1px solid #ddd">⏳ Triple Long Press</td>
<td style="padding:8px;border:1px solid #ddd">Triggers <b>triple long-press event</b></td>
</tr>
<tr>
<td style="padding:8px;border:1px solid #ddd">📐 Hold & Keep Pressing</td>
<td style="padding:8px;border:1px solid #ddd">Enter special mode: read 3‑axis accelerometer data and reset encoder count</td>
</tr>
</tbody>
</table>
</div>

<hr style="border:none;border-top:1px solid #ccc;margin:30px 0" />

<div style="background:#f8f9fa;padding:20px;border-radius:10px;border-left:5px solid #2a9d8f">
<h3 style="color:#2a9d8f;margin-top:0">User Guide</h3>
<div style="background:#ffffff;border:1px solid #eee;padding:18px;border-radius:10px">
<h4>1️⃣ Firmware Flashing</h4>
<p>Flash firmware using the <strong>WCH_TOOL</strong> utility.</p>
<p>Features: simple operation | batch support | suitable for mass production</p>
<p>Firmware file is included in attachments.</p>
<div align="center" style="margin-top:15px">
<img width="742" height="451" alt="image" src="https://github.com/user-attachments/assets/cc4ca9d0-dcba-415f-8e3c-df9998462bfb" />

</div>
</div>

<div style="background:#ffffff;border:1px solid #eee;padding:18px;border-radius:10px">
<h4>2️⃣ Use with HomeAssistant</h4>
<p>After flashing, add the device in <strong>Home Assistant (HA)</strong>.</p>
<p>The device is usually <strong>auto‑discovered</strong> under the name <strong>WKnob</strong>.</p>
<div align="center" style="margin-top:15px">
<img width="742" height="278" alt="image" src="https://github.com/user-attachments/assets/5679e56e-59d9-409c-b617-832425754b52" />

</div>
<p>Once added, you can view real‑time sensor data, create automations, and link with other HA devices.</p>
<div align="center" style="margin-top:15px">
<img width="742" height="656" alt="image" src="https://github.com/user-attachments/assets/c336ae48-1cf1-44c7-80d2-785d66b7e84d" />

</div>
</div>
</div>

<hr style="border:none;border-top:1px solid #ccc;margin:30px 0" />

<div style="background:#f8f9fa;padding:20px;border-radius:10px;border-left:5px solid #2a9d8f">
<h3 style="color:#2a9d8f;margin-top:0">Communication Mechanism</h3>
<p>Default strategy:<br />
Idle state: broadcast every 10 seconds<br />
Action triggered: broadcast 10 times per second<br />
Trigger state lasts <strong>2–5 seconds</strong>.<br />
Balances <strong>low power consumption</strong> and <strong>fast response</strong>.</p>
</div>

<hr style="border:none;border-top:1px solid #ccc;margin:30px 0" />

<div style="background:#f8f9fa;padding:18px 20px;border-radius:10px;border-left:4px solid #2a9d8f;font-family:system-ui,sans-serif;max-width:720px">
<h3 style="color:#2a9d8f;margin:0 0 12px 0">Related HA Projects</h3>
<ul style="list-style:none;padding:0;margin:0;line-height:1.6">
<li style="margin-bottom:10px"><b>🔵 HA BLE Door & Light Sensor / LMsensor</b><br /><a href="https://oshwhub.com/bitshen/bthome" style="color:#2a9d8f;text-decoration:none">https://oshwhub.com/bitshen/bthome</a></li>
<li style="margin-bottom:10px"><b>🟣 HA Voice Assistant / AIO</b><br /><a href="https://oshwhub.com/bitshen/esphome-ai" style="color:#2a9d8f;text-decoration:none">https://oshwhub.com/bitshen/esphome-ai</a></li>
<li style="margin-bottom:10px"><b>🟢 HA BLE Thermometer & Hygrometer</b><br /><a href="https://oshwhub.com/bitshen/shi-wai-wen-du-ji_copy" style="color:#2a9d8f;text-decoration:none">https://oshwhub.com/bitshen/shi-wai-wen-du-ji_copy</a></li>
<li style="margin-bottom:10px"><b>🟡 HA Desktop Sensor / CliQ</b><br /><a href="https://oshwhub.com/bitshen/guang-wen-ji-zhuo-mian-bai-jian" style="color:#2a9d8f;text-decoration:none">https://oshwhub.com/bitshen/guang-wen-ji-zhuo-mian-bai-jian</a></li>
<li style="margin-bottom:10px"><b>🟠 HA BLE Gateway VerA</b><br /><a href="https://oshwhub.com/bitshen/esp32-c3-mi-ni-lan-ya-wang-guan_copy" style="color:#2a9d8f;text-decoration:none">https://oshwhub.com/bitshen/esp32-c3-mi-ni-lan-ya-wang-guan_copy</a></li>
<li><b>🔷 HA BLE Gateway VerB</b><br /><a href="https://oshwhub.com/bitshen/esp32-c3-mi-ni-lan-ya-wang-guan_copy_copy" style="color:#2a9d8f;text-decoration:none">https://oshwhub.com/bitshen/esp32-c3-mi-ni-lan-ya-wang-guan_copy_copy</a></li>
</ul>
</div>

<hr style="border:none;border-top:1px solid #ccc;margin:30px 0" />

<div style="background:#f8f9fa;padding:20px;border-radius:10px;border-left:5px solid #2a9d8f">
<h3 style="color:#2a9d8f;margin-top:0">Components & Parts</h3>
<p>SMT encoder — pay attention to height when purchasing</p>
<img width="820" height="247" alt="image" src="https://github.com/user-attachments/assets/8a972152-f8cc-4461-8aee-42e5f387b035" />

<p>Aluminum alloy knob</p>
<img width="820" height="249" alt="image" src="https://github.com/user-attachments/assets/9f9143c1-d979-4bd4-8231-7c2651f549f7" />

<p>Enclosure design in attachments — uses <strong>heat‑set inserts: M3 × 4 × 3</strong></p>
<div style="text-align:center;margin-top:15px">
<img width="820" height="606" alt="image" src="https://github.com/user-attachments/assets/30840805-7122-409d-b56c-b74e63cb8046" />

<img width="820" height="601" alt="image" src="https://github.com/user-attachments/assets/6c9b79aa-bf5e-45a9-893d-083685ee0294" />
  <img width="910" height="718" alt="image" src="https://github.com/user-attachments/assets/24af3699-df83-459e-abb2-e6193c36faaf" />

</div>
</div>

<div style="background:#f1f1f1;padding:25px;border-radius:10px;border-left:6px solid #2a9d8f;margin-top:40px">
<p>Afterword —<br />
This is probably <strong>my last project as a student</strong>.<br />
I’m not sure if I’ll still have time for these fun personal projects after graduation. Maybe future projects will only be for work and profit. I’m planning one more keyboard project as a graduation gift this year.<br />
Started well, ended well.</p>
</div>

</div>
