<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');

require_once __DIR__ . '/../includes/db.php';

$rfid_uid = $_POST['rfid_uid'] ?? '';
$full_name = $_POST['full_name'] ?? '';
$age = $_POST['age'] ?? null;
$gender = $_POST['gender'] ?? null;
$blood_type = $_POST['blood_type'] ?? null;
$medical_history = $_POST['medical_history'] ?? null;
$emergency_contact = $_POST['emergency_contact'] ?? null;

if (empty($rfid_uid) || empty($full_name)) {
    echo json_encode(['success' => false, 'error' => 'RFID and Full Name are required']);
    exit;
}

try {
    // Check if RFID already exists
    $stmt = $pdo->prepare("SELECT id FROM patients WHERE rfid_uid = ?");
    $stmt->execute([$rfid_uid]);
    if ($stmt->fetch()) {
        echo json_encode(['success' => false, 'error' => 'RFID already registered']);
        exit;
    }

    // Insert patient
    $stmt = $pdo->prepare("
        INSERT INTO patients (rfid_uid, full_name, age, gender, blood_type, medical_history, emergency_contact)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    ");

    $stmt->execute([
        $rfid_uid,
        $full_name,
        $age ?: null,
        $gender ?: null,
        $blood_type ?: null,
        $medical_history ?: null,
        $emergency_contact ?: null
    ]);

    echo json_encode(['success' => true, 'id' => $pdo->lastInsertId()]);
} catch (Exception $e) {
    echo json_encode(['success' => false, 'error' => $e->getMessage()]);
}
?>
