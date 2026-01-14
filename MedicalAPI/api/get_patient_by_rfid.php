<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');

require_once __DIR__ . '/../includes/db.php';

$rfid = $_GET['rfid'] ?? '';

if (empty($rfid)) {
    echo json_encode(['error' => 'RFID required']);
    exit;
}

try {
    $stmt = $pdo->prepare("SELECT * FROM patients WHERE rfid_uid = ?");
    $stmt->execute([$rfid]);
    $patient = $stmt->fetch();

    if (!$patient) {
        echo json_encode(['error' => 'not found']);
        exit;
    }

    // Clear old sessions and create new one
    $pdo->exec("DELETE FROM active_session");
    $stmt = $pdo->prepare("INSERT INTO active_session (patient_id, session_start) VALUES (?, NOW())");
    $stmt->execute([$patient['id']]);

    echo json_encode(['success' => true, 'patient' => $patient]);
} catch (Exception $e) {
    echo json_encode(['error' => $e->getMessage()]);
}
?>
