using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.XR.Interaction.Toolkit;
using System.Collections;

public class VRHttpTrigger : MonoBehaviour
{
    [Header("HTTP Settings")]
    public string endpointUrl = "http://192.168.1.222/trigger";
    public string jsonPayload = "{\"message\":\"VR Triggered\"}";
    public bool usePost = true;

    private UnityEngine.XR.Interaction.Toolkit.Interactables.XRBaseInteractable interactable;

    void Awake()
    {
        interactable = GetComponent<UnityEngine.XR.Interaction.Toolkit.Interactables.XRBaseInteractable>();
        interactable.selectEntered.AddListener(OnInteract);
        
    }

    private void OnInteract(SelectEnterEventArgs args)
    {
        StartCoroutine(SendRequest());
    }

    IEnumerator SendRequest()
    {
        string json = "{\"message\":\"hello\"}";
        byte[] bodyRaw = System.Text.Encoding.UTF8.GetBytes(json);

        UnityWebRequest request =
            new UnityWebRequest(endpointUrl, "POST");

        request.uploadHandler = new UploadHandlerRaw(bodyRaw);
        request.downloadHandler = new DownloadHandlerBuffer();
        request.SetRequestHeader("Content-Type", "application/json");

        yield return request.SendWebRequest();

        Debug.Log("Result: " + request.result);
        Debug.Log("Error: " + request.error);
        Debug.Log("Code: " + request.responseCode);
        Debug.Log("Response: " + request.downloadHandler.text);
    }
}