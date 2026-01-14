// Load patients on page load
loadPatients();

// Form submissions
document.getElementById('addPatientForm').addEventListener('submit', function(e) {
    e.preventDefault();
    addPatient();
});

document.getElementById('editPatientForm').addEventListener('submit', function(e) {
    e.preventDefault();
    updatePatient();
});

function loadPatients() {
    fetch('/medical/api/get_patients.php')
        .then(res => res.json())
        .then(data => {
            if (data.success && data.patients) {
                displayPatients(data.patients);
            }
        })
        .catch(err => console.error('Error loading patients:', err));
}

function displayPatients(patients) {
    const tbody = document.getElementById('patientTableBody');
    
    if (patients.length === 0) {
        tbody.innerHTML = '<tr><td colspan="7" style="text-align: center; color: #999;">No patients registered yet</td></tr>';
        return;
    }

    tbody.innerHTML = patients.map(patient => `
        <tr>
            <td><strong>${patient.rfid_uid}</strong></td>
            <td>${patient.full_name}</td>
            <td>${patient.age || '--'}</td>
            <td>${patient.gender || '--'}</td>
            <td>${patient.blood_type || '--'}</td>
            <td>${patient.emergency_contact || '--'}</td>
            <td>
                <button class="btn btn-view" onclick="viewHistory(${patient.id}, '${patient.full_name}')">History</button>
                <button class="btn" style="background: #ffc107; color: white;" onclick='editPatient(${JSON.stringify(patient)})'>Edit</button>
                <button class="btn" style="background: #dc3545; color: white;" onclick="deletePatient(${patient.id}, '${patient.full_name}')">Delete</button>
            </td>
        </tr>
    `).join('');
}

function addPatient() {
    const formData = new FormData();
    formData.append('rfid_uid', document.getElementById('rfidUid').value.trim());
    formData.append('full_name', document.getElementById('fullName').value.trim());
    formData.append('age', document.getElementById('age').value);
    formData.append('gender', document.getElementById('gender').value);
    formData.append('blood_type', document.getElementById('bloodType').value);
    formData.append('emergency_contact', document.getElementById('emergencyContact').value.trim());
    formData.append('medical_history', document.getElementById('medicalHistory').value.trim());

    fetch('/medical/api/add_patient.php', {
        method: 'POST',
        body: formData
    })
    .then(res => res.json())
    .then(data => {
        if (data.success) {
            showMessage('Patient added successfully!', 'success');
            document.getElementById('addPatientForm').reset();
            loadPatients();
        } else {
            showMessage('Error: ' + data.error, 'error');
        }
    })
    .catch(err => showMessage('Error: ' + err, 'error'));
}

function editPatient(patient) {
    document.getElementById('editPatientId').value = patient.id;
    document.getElementById('editFullName').value = patient.full_name;
    document.getElementById('editAge').value = patient.age || '';
    document.getElementById('editGender').value = patient.gender || '';
    document.getElementById('editBloodType').value = patient.blood_type || '';
    document.getElementById('editEmergencyContact').value = patient.emergency_contact || '';
    document.getElementById('editMedicalHistory').value = patient.medical_history || '';
    
    document.getElementById('editModal').classList.add('active');
}

function closeEditModal() {
    document.getElementById('editModal').classList.remove('active');
}

function updatePatient() {
    const formData = new FormData();
    formData.append('id', document.getElementById('editPatientId').value);
    formData.append('full_name', document.getElementById('editFullName').value.trim());
    formData.append('age', document.getElementById('editAge').value);
    formData.append('gender', document.getElementById('editGender').value);
    formData.append('blood_type', document.getElementById('editBloodType').value);
    formData.append('emergency_contact', document.getElementById('editEmergencyContact').value.trim());
    formData.append('medical_history', document.getElementById('editMedicalHistory').value.trim());

    fetch('/medical/api/update_patient.php', {
        method: 'POST',
        body: formData
    })
    .then(res => res.json())
    .then(data => {
        if (data.success) {
            showEditMessage('Patient updated successfully!', 'success');
            setTimeout(() => {
                closeEditModal();
                loadPatients();
            }, 1000);
        } else {
            showEditMessage('Error: ' + data.error, 'error');
        }
    })
    .catch(err => showEditMessage('Error: ' + err, 'error'));
}

function deletePatient(id, name) {
    if (!confirm(`Are you sure you want to delete patient "${name}"?\n\nThis will also delete all their sensor readings and history.`)) {
        return;
    }

    const formData = new FormData();
    formData.append('id', id);

    fetch('/medical/api/delete_patient.php', {
        method: 'POST',
        body: formData
    })
    .then(res => res.json())
    .then(data => {
        if (data.success) {
            alert('Patient deleted successfully!');
            loadPatients();
        } else {
            alert('Error: ' + data.error);
        }
    })
    .catch(err => alert('Error: ' + err));
}

function viewHistory(patientId, patientName) {
    fetch(`/medical/api/get_patient_history.php?id=${patientId}`)
        .then(res => res.json())
        .then(data => {
            if (data.success && data.readings) {
                displayHistory(patientName, data.readings);
            }
        })
        .catch(err => console.error('Error loading history:', err));
}

function displayHistory(patientName, readings) {
    document.getElementById('historyPatientName').textContent = patientName;
    
    const content = document.getElementById('historyContent');
    
    if (readings.length === 0) {
        content.innerHTML = '<p style="text-align: center; color: #999;">No readings recorded yet</p>';
    } else {
        let html = '<table style="width: 100%; border-collapse: collapse;">';
        html += '<thead><tr style="background: #f8f9fa;">';
        html += '<th style="padding: 10px; border: 1px solid #dee2e6; text-align: left;">Time</th>';
        html += '<th style="padding: 10px; border: 1px solid #dee2e6; text-align: left;">ECG</th>';
        html += '<th style="padding: 10px; border: 1px solid #dee2e6; text-align: left;">HR (BPM)</th>';
        html += '<th style="padding: 10px; border: 1px solid #dee2e6; text-align: left;">SpO2 (%)</th>';
        html += '<th style="padding: 10px; border: 1px solid #dee2e6; text-align: left;">Body Temp (°C)</th>';
        html += '<th style="padding: 10px; border: 1px solid #dee2e6; text-align: left;">Room Temp (°C)</th>';
        html += '<th style="padding: 10px; border: 1px solid #dee2e6; text-align: left;">Humidity (%)</th>';
        html += '<th style="padding: 10px; border: 1px solid #dee2e6; text-align: left;">Air Quality (PPM)</th>';
        html += '</tr></thead><tbody>';

        readings.forEach(reading => {
            const time = new Date(reading.timestamp).toLocaleString();
            html += '<tr style="border-bottom: 1px solid #dee2e6;">';
            html += `<td style="padding: 10px; border: 1px solid #dee2e6;">${time}</td>`;
            html += `<td style="padding: 10px; border: 1px solid #dee2e6;">${reading.ecg_value || '--'}</td>`;
            html += `<td style="padding: 10px; border: 1px solid #dee2e6;">${reading.heart_rate || '--'}</td>`;
            html += `<td style="padding: 10px; border: 1px solid #dee2e6;">${reading.spo2 || '--'}</td>`;
            html += `<td style="padding: 10px; border: 1px solid #dee2e6;">${reading.body_temp ? parseFloat(reading.body_temp).toFixed(1) : '--'}</td>`;
            html += `<td style="padding: 10px; border: 1px solid #dee2e6;">${reading.room_temp ? parseFloat(reading.room_temp).toFixed(1) : '--'}</td>`;
            html += `<td style="padding: 10px; border: 1px solid #dee2e6;">${reading.humidity ? parseFloat(reading.humidity).toFixed(0) : '--'}</td>`;
            html += `<td style="padding: 10px; border: 1px solid #dee2e6;">${reading.air_quality || '--'}</td>`;
            html += '</tr>';
        });

        html += '</tbody></table>';
        content.innerHTML = html;
    }

    document.getElementById('historyModal').classList.add('active');
}

function closeHistoryModal() {
    document.getElementById('historyModal').classList.remove('active');
}

function showMessage(msg, type) {
    const msgDiv = document.getElementById('formMessage');
    msgDiv.textContent = msg;
    msgDiv.style.color = type === 'error' ? '#dc3545' : '#28a745';
}

function showEditMessage(msg, type) {
    const msgDiv = document.getElementById('editMessage');
    msgDiv.textContent = msg;
    msgDiv.style.color = type === 'error' ? '#dc3545' : '#28a745';
}

// Reload patients every 5 seconds
setInterval(loadPatients, 5000);
