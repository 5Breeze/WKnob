![Snipaste_2026-03-06_19-51-14](https://github.com/user-attachments/assets/132e63fa-159f-4a7b-aed6-850960ce8435)

<div style="font-family:sans-serif;max-width:900px;margin:auto;color:#333;line-height:1.7">

<div style="background:linear-gradient(135deg,#2a9d8f,#264653);padding:32px;border-radius:14px;color:white;text-align:center;margin-bottom:30px">
<h1 style="margin:0">WKnob Wireless Knob</h1>
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
<img src="https://image.lceda.cn/oshwhub/pullImage/353b34026d5d416d9a0d3ffef39fc01a.png" width="500" style="border-radius:8px;cursor:pointer" />
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
<img src="https://image.lceda.cn/oshwhub/pullImage/f9eaa7fd5a03474e9db425137d39f74d.png" width="500" style="border-radius:8px" />
</div>
</div>

<div style="background:#ffffff;border:1px solid #eee;padding:18px;border-radius:10px">
<h4>2️⃣ Use with HomeAssistant</h4>
<p>After flashing, add the device in <strong>Home Assistant (HA)</strong>.</p>
<p>The device is usually <strong>auto‑discovered</strong> under the name <strong>WKnob</strong>.</p>
<div align="center" style="margin-top:15px">
<img src="https://image.lceda.cn/oshwhub/pullImage/c6c86faa91e244c9a85e51cd99ad4f30.png" width="500" style="border-radius:8px" />
</div>
<p>Once added, you can view real‑time sensor data, create automations, and link with other HA devices.</p>
<div align="center" style="margin-top:15px">
<img src="https://image.lceda.cn/oshwhub/pullImage/75fe337a1f894c80a9c90e519358172f.png" width="500" style="border-radius:8px" />
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
<img src="https://image.lceda.cn/oshwhub/pullImage/e533f88ac611416c9091a49afb1254e9.png" width="400" style="border-radius:8px;margin-bottom:15px" />
<p>Aluminum alloy knob</p>
<img src="https://image.lceda.cn/oshwhub/pullImage/6c143a5f28094afc9b642ea547c06b6a.png" width="400" style="border-radius:8px;margin-bottom:15px" />
<p>Enclosure design in attachments — uses <strong>heat‑set inserts: M3 × 4 × 3</strong></p>
<div style="text-align:center;margin-top:15px">
<img src="https://image.lceda.cn/oshwhub/pullImage/1d2ced9908f7459fa220dbb35b8e6d70.png" width="400" style="border-radius:8px;margin-bottom:10px" />
<img src="https://image.lceda.cn/oshwhub/pullImage/d242140c2741476cad1327bc8f4277fa.png" width="400" style="border-radius:8px" />
</div>
</div>

<div style="background:#f1f1f1;padding:25px;border-radius:10px;border-left:6px solid #2a9d8f;margin-top:40px">
<p>Afterword —<br />
This is probably <strong>my last project as a student</strong>.<br />
I’m not sure if I’ll still have time for these fun personal projects after graduation. Maybe future projects will only be for work and profit. I’m planning one more keyboard project as a graduation gift this year.<br />
Started well, ended well.</p>
</div>

</div>
