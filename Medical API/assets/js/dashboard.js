// ECG Chart Setup with FIXED dimensions
const ctx = document.getElementById('ecgChart').getContext('2d');
const ecgChart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'ECG',
            data: [],
            borderColor: '#667eea',
            backgroundColor: 'rgba(102, 126, 234, 0.1)',
            borderWidth: 2,
            tension: 0.4,
            pointRadius: 0,
            fill: true
        }]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false, // CRITICAL: allows fixed height
        animation: false,
        scales: {
            x: {
                display: false,
                grid: {
                    display: false
                }
            },
            y: {
                beginAtZero: false,
                min: 0,
                max: 4096,
                grid: {
                    color: 'rgba(0, 0, 0, 0.05)'
                },
                ticks: {
                    stepSize: 1000
                }
            }
        },
        plugins: {
            legend: {
                display: false
            },
            tooltip: {
                enabled: false
            }
        },
        interaction: {
            intersect: false,
            mode: 'index'
        }
    }
});

// Global state
let currentPatient = null;
let isConnected = false;
const maxDataPoints = 80;

// Fetch latest readings every second
setInterval(fetchLatestData, 1000);

// Initial fetch
fetchLatestData();

function fetchLatestData() {
    fetch('/medical/api/latest_readings.php')
        .then(res => res.json())
        .then(data => {
            if (data.error) {
                // No session or error - disconnect
                if (isConnected) {
                    // Only clear if we were previously connected
                    clearPatientInfo();
                }
                updateStatus(false);
                isConnected = false;
                return;
            }

            if (data.patient && data.reading) {
                // Valid session with data
                isConnected = true;
                updateStatus(true, data.patient.name);
                updatePatientInfo(data.patient);
                updateVitals(data.reading);
                
                // ONLY update ECG if we have a valid reading
                if (data.reading.ecg_value && data.reading.ecg_value > 0) {
                    updateECG(data.reading.ecg_value);
                }
            }
        })
        .catch(err => {
            console.error('Fetch error:', err);
            updateStatus(false);
            isConnected = false;
        });
}

function updateStatus(connected, patientName = '') {
    const statusDot = document.getElementById('statusDot');
    const statusText = document.getElementById('statusText');
    
    if (connected) {
        statusDot.classList.add('connected');
        statusText.textContent = `Monitoring: ${patientName}`;
    } else {
        statusDot.classList.remove('connected');
        statusText.textContent = 'Waiting for patient...';
    }
}

function updatePatientInfo(patient) {
    const patientCard = document.getElementById('patientCard');
    patientCard.classList.add('active');
    
    document.getElementById('patientName').textContent = patient.name || '--';
    document.getElementById('patientAge').textContent = patient.age || '--';
    document.getElementById('patientGender').textContent = patient.gender || '--';
    document.getElementById('patientBlood').textContent = patient.blood_type || '--';
    document.getElementById('patientRFID').textContent = patient.rfid_uid || '--';
    
    currentPatient = patient;
}

function clearPatientInfo() {
    const patientCard = document.getElementById('patientCard');
    patientCard.classList.remove('active');
    
    document.getElementById('patientName').textContent = '--';
    document.getElementById('patientAge').textContent = '--';
    document.getElementById('patientGender').textContent = '--';
    document.getElementById('patientBlood').textContent = '--';
    document.getElementById('patientRFID').textContent = '--';
    
    // Clear vitals
    document.getElementById('heartRate').textContent = '--';
    document.getElementById('spo2').textContent = '--';
    document.getElementById('bodyTemp').textContent = '--';
    document.getElementById('roomTemp').textContent = '--';
    document.getElementById('humidity').textContent = '--';
    document.getElementById('airQuality').textContent = '--';
    
    // Clear and reset ECG chart
    ecgChart.data.labels = [];
    ecgChart.data.datasets[0].data = [];
    ecgChart.update('none');
    
    currentPatient = null;
}

function updateVitals(reading) {
    // Heart Rate
    const hr = reading.heart_rate || 0;
    document.getElementById('heartRate').textContent = hr > 0 ? hr : '--';
    
    // SpO2
    const spo2 = reading.spo2 || 0;
    document.getElementById('spo2').textContent = spo2 > 0 ? spo2 : '--';
    
    // Body Temperature
    const bodyTemp = parseFloat(reading.body_temp) || 0;
    document.getElementById('bodyTemp').textContent = bodyTemp > 0 ? bodyTemp.toFixed(1) : '--';
    
    // Room Temperature
    const roomTemp = parseFloat(reading.room_temp) || 0;
    document.getElementById('roomTemp').textContent = roomTemp > 0 ? roomTemp.toFixed(1) : '--';
    
    // Humidity
    const humidity = parseFloat(reading.humidity) || 0;
    document.getElementById('humidity').textContent = humidity > 0 ? humidity.toFixed(0) : '--';
    
    // Air Quality
    const airQuality = reading.air_quality || 0;
    document.getElementById('airQuality').textContent = airQuality > 0 ? airQuality : '--';
}

function updateECG(value) {
    // CRITICAL: Only update if we have a valid connection
    if (!isConnected || !value || value <= 0) {
        return;
    }
    
    const chart = ecgChart;
    
    chart.data.labels.push('');
    chart.data.datasets[0].data.push(value);
    
    // Keep only last 80 points
    if (chart.data.labels.length > maxDataPoints) {
        chart.data.labels.shift();
        chart.data.datasets[0].data.shift();
    }
    
    chart.update('none');
}
