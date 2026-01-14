<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');

require_once __DIR__ . '/../includes/db.php';

try {
    $stmt = $pdo->query("SELECT patient_id FROM active_session LIMIT 1");
    $session = $stmt->fetch();

    if (!$session) {
        echo json_encode(['error' => 'no session']);
        exit;
    }

    // Get FULL patient info including age, gender, blood_type
    $stmt = $pdo->prepare("SELECT id, full_name as name, rfid_uid, age, gender, blood_type FROM patients WHERE id = ?");
    $stmt->execute([$session['patient_id']]);
    $patient = $stmt->fetch();

    $stmt = $pdo->prepare("
        SELECT * FROM sensor_readings 
        WHERE patient_id = ? 
        ORDER BY timestamp DESC 
        LIMIT 1
    ");
    $stmt->execute([$session['patient_id']]);
    $reading = $stmt->fetch();

    echo json_encode([
        'patient' => $patient,
        'reading' => $reading ?: null
    ]);
} catch (Exception $e) {
    echo json_encode(['error' => $e->getMessage()]);
}
?>
