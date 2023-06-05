#include <iostream>
#include <vector>

using std::cout; using std::endl;
using std::vector;

enum modeLPFO { kKaon, kPion };
TString LPFOName[2]  = { "K", "Pi" };
TString LPFOLabel[2] = { "K", "#pi" };

/*
class SampleMode
{
  public:
    SampleMode(){}
    ~SampleMode(){}

    TString getFileName(){
      return fileName;
    }

    TString getSampleName(){

      toa = fileName.Tokenize(".");
      for ( int i = 0; i < toa->GetEntries(); i++ ){
        TString token = ((TObjString*)toa->At(i))->GetString();
        if ( token.EqualTo("uu") || token.EqualTo("dd") || token.EqualTo("ss") ){
          sampleName = token;
        }
      }

      return sampleName;
    }

    void setFileName(TString fn){
      fileName = fn;
    }

    TFile *getFile(){
      file = new TFile(fileName.Data(),"READ");
      return file;
    }

  private:
    TString   fileName;
    TString   sampleName;
    TFile     *file;
    TObjArray *toa;

};
*/

void BinNormal(TH1F *h)
{
  const Int_t nbins = h->GetNbinsX();
  for (int ibin=1; ibin<=nbins; ibin++){
    Float_t binc = h->GetBinContent(ibin);
    Float_t binw = h->GetBinWidth(ibin);
    Float_t bin_normal = binc / binw;
    h->SetBinContent(ibin,bin_normal);
  }

}

void Normalize(TH1F *h)
{
  // h->Scale( 1.0 / h->GetEntries() );
  const Int_t nbins = h->GetNbinsX();
  Int_t nbins4 = nbins / 4;
  Int_t int_high = (nbins / 2) + nbins4;
  Int_t int_low  = (nbins / 2 + 1) - nbins4;
  h->Scale( 1.0 / h->Integral(int_low,int_high) );
}

TH1F *Normalize2Gen(TString name, TH1F *h, TH1F *h_gen)
{
  TH1F *h_norm = (TH1F*)h->Clone();
  h_norm->SetName(name.Data());
	double intCosReco = h_norm->Integral(20,80);
	double intCosGen  = h_gen->Integral(20,80);
  h_norm->Scale( intCosGen / intCosReco );
  return h_norm;
}

void StyleHist(TH1F *h, Color_t col)
{
  h->SetLineWidth(3);
  h->SetLineColor(col);
  h->SetFillStyle(3002);
  h->SetFillColor(col);
}

void StylePad(TPad *pad, Float_t t, Float_t b, Float_t r, Float_t l)
{
  pad->SetGrid(1,1);
  if(t) pad->SetTopMargin(t);
  if(b) pad->SetBottomMargin(b);
  if(r) pad->SetRightMargin(r);
  if(l) pad->SetLeftMargin(l);
  pad->Draw();
  pad->cd();

}

vector<Float_t> GetP( TH1F * h_accepted, TH1F * h_rejected )
{
  const Int_t nbins = h_accepted->GetNbinsX();
  vector<Float_t> result_error;
  vector<Float_t> result;

  for (int i = 1; i < nbins / 2 + 1; i++)
  {

    std::vector<float> result_j;
    for (int i1 = -1; i1 < 2; i1 += 2)
    {
      for (int i2 = -1; i2 < 2; i2 += 2)
      {
        for (int i3 = -1; i3 < 2; i3 += 2)
        {
          float accepted = h_accepted->GetBinContent(nbins + 1 - i) + i1 * sqrt(h_accepted->GetBinContent(nbins + 1 - i));
          float rejected = h_rejected->GetBinContent(nbins + 1 - i) + i2 * sqrt(h_rejected->GetBinContent(nbins + 1 - i));
          accepted += h_accepted->GetBinContent(i) + i3 * sqrt(h_accepted->GetBinContent(i));

          float a = 1;
          float b = -1;
          float c = rejected / (2 * (accepted + rejected));
          float p = (0.5 / a) * (-b + sqrt(b * b - 4 * a * c));
          float p2 = (0.5 / a) * (-b - sqrt(b * b - 4 * a * c));

          if (p > 0.99)
            p = 0;
          if (p2 > 0.99)
            p2 = 0;
          if (p > 0 || p2 > 0)
            result_j.push_back(max(p, p2));
        }
      }
    }

    float average = 0;
    float n = 0;
    for (unsigned j = 0; j < result_j.size(); j++)
    {
      if (result_j.at(j) > 0)
      {
        average += result_j.at(j);
        n++;
      }
    }
    average /= n;

    if ( (average != 0) && (average == average) )
    {
      result.push_back(average);
      float std_dev = 0;
      for (unsigned j = 0; j < result_j.size(); j++)
      {
        if (result_j.at(j) > 0)
        {
          std_dev += pow(result_j.at(j) - average, 2);
        }
      }
      std_dev = sqrt(std_dev / (n - 1));
      result_error.push_back(std_dev);
    }
    else
    {
      result_error.push_back(0);
      result.push_back(0);
    }
  }

  for (unsigned i = 0; i < result_error.size(); i++)
  {
    if (result_error.at(i) > 0)
      result.push_back(result_error.at(i));
    else
      result.push_back(0);
  }

  return result;

}

TH1F * CorrectHist( TString name, TH1F * h_reco, vector<Float_t> p_vec)
{
  const Int_t nbins = h_reco->GetNbinsX();

  TH1F *corrected = new TH1F(name.Data(), name.Data(), 100,-1,1);
  corrected->Sumw2();
  for (int i = 1; i < nbins / 2 + 1; i++)
  {
    float p = p_vec.at(i - 1);
    // float p = 0.7;
    float q = 1 - p;
    float weight = (p * p + q * q) / (q * q * q * q - p * p * p * p);

    // calcualte average
    float av_i = 0;
    float av_41i = 0;
    float n = 0;
    for (int i1 = -1; i1 < 2; i1 += 1)
    {
      for (int i2 = -1; i2 < 2; i2 += 1)
      {
        float nm_reco_error = h_reco->GetBinContent(i) + i1 * h_reco->GetBinError(i);
        float np_reco_error = h_reco->GetBinContent(nbins + 1 - i) + i2 * h_reco->GetBinError(nbins + 1 - i);
        av_i += (np_reco_error * q * q - nm_reco_error * p * p) * weight;
        av_41i += -(np_reco_error * p * p - nm_reco_error * q * q) * weight;
        n++;
      }
    }
    av_i /= n;
    av_41i /= n;
    corrected->SetBinContent(i, av_i);
    corrected->SetBinContent(nbins + 1 - i, av_41i);
    // cout << "val: " << av_41i << ", p: " << p << endl;

    // calculate error
    float error_i = 0;
    float error_41i = 0;
    n = 0;
    for (int i1 = -1; i1 < 2; i1 += 1)
    {
      for (int i2 = -1; i2 < 2; i2 += 1)
      {
        float nm_reco_error = h_reco->GetBinContent(i) + i1 * h_reco->GetBinError(i);
        float np_reco_error = h_reco->GetBinContent(nbins + 1 - i) + i2 * h_reco->GetBinError(nbins + 1 - i);
        error_i += pow((np_reco_error * q * q - nm_reco_error * p * p) * weight - av_i, 2);
        error_41i += pow(-(np_reco_error * p * p - nm_reco_error * q * q) * weight - av_41i, 2);
        n++;
      }
    }
    error_i = sqrt(error_i / (n - 1.));
    error_41i = sqrt(error_41i / (n - 1.));
    corrected->SetBinError(i, error_i);
    corrected->SetBinError(nbins + 1 - i, error_41i);
  }

  return corrected;

}

TH1F * Efficiency_Correction( TH1F * h, TString name, TFile * file )
{
  TH1F *h_gen_q_qcos  = (TH1F*) file->Get("h_gen_K_cos");
  TH1F *h_reco_Pi_qcos = (TH1F*) file->Get("h_reco_Pi_cos");

  TH1F *h_eff = (TH1F*) h_reco_Pi_qcos->Clone();
  h_eff->Divide(h_gen_q_qcos);

  // TCanvas *c_eff = TCanvas();

  if( h->GetNbinsX() != h_eff->GetNbinsX() ) throw std::logic_error("Error");

  Int_t nbins = h_eff->GetNbinsX();
  TH1F *corrected = new TH1F(name.Data(), "corrected", 100,-1,1);
  corrected->Sumw2();
  for (int ibin = 1; ibin < nbins + 1; ibin++){

    Float_t binc_h   = h->GetBinContent(ibin);
    Float_t binc_eff = h_eff->GetBinContent(ibin);

    corrected->SetBinContent(ibin,binc_h / binc_eff);

  }

  return corrected;

}

TH1F * Efficiency_Correction2( TH1F * h, TString name, TFile * file )
{
  TH1F *h_gen_N_K_cos  = (TH1F*) file->Get("h_gen_N_K_cos");
  TH1F *h_reco_N_K_cos = (TH1F*) file->Get("h_reco_N_K_cos");
  TH1F *h_N_K_corr_cos = (TH1F*) file->Get("h_N_K_corr_cos");

  TH1F *h_stable_cos = (TH1F*) h_N_K_corr_cos->Clone();
  TH1F *h_purity_cos = (TH1F*) h_N_K_corr_cos->Clone();
  h_stable_cos->Divide(h_gen_N_K_cos);
  h_purity_cos->Divide(h_reco_N_K_cos);

  if( h->GetNbinsX() != h_stable_cos->GetNbinsX() ) throw std::logic_error("Error");
  if( h->GetNbinsX() != h_purity_cos->GetNbinsX() ) throw std::logic_error("Error");

  TH1F *h_weight = (TH1F*) h_purity_cos->Clone();
  h_weight->Divide(h_stable_cos);

  Int_t nbins = h_stable_cos->GetNbinsX();
  TH1F *corrected = new TH1F(name.Data(), "corrected", 100,-1,1);
  corrected->Sumw2();
  for (int ibin = 1; ibin < nbins + 1; ibin++){

    Float_t binc_h   = h->GetBinContent(ibin);
    Float_t binc_eff = h_weight->GetBinContent(ibin);

    corrected->SetBinContent(ibin,binc_h * binc_eff);

  }

  return corrected;

}

void pq_polar( TFile *file, modeLPFO mlpfo )
{
  TString LPFO = LPFOName[mlpfo];

  TH1F *h_gen_q_qcos  = (TH1F*) file->Get("h_gen_q_qcos");
  TH1F *h_reco_LPFO_scos = (TH1F*) file->Get("h_reco_" + LPFO + "_scos");
  TH1F *h_reco_LPFO_qcos = (TH1F*) file->Get("h_reco_" + LPFO + "_qcos");
  TH1F *h_cheat_LPFO_qcos = (TH1F*) file->Get("h_cheat_" + LPFO + "_qcos");

  // efficiency correction
  // TH1F *h_reco_LPFO_scos_eff_corr = Efficiency_Correction2(h_reco_LPFO_scos,"scos_corr",file);
  // TH1F *h_reco_LPFO_qcos_eff_corr = Efficiency_Correction2(h_reco_LPFO_qcos,"qcos_corr",file);
  TH1F *h_reco_LPFO_scos_eff_corr = (TH1F*) h_reco_LPFO_scos->Clone();
  TH1F *h_reco_LPFO_qcos_eff_corr = (TH1F*) h_reco_LPFO_qcos->Clone();

  // used for pq correction
  TH1F *h_acc_LPFO_cos  = (TH1F*) file->Get("pq/h_acc_" + LPFO + LPFO + "_cos");
  TH1F *h_rej_LPFO_cos  = (TH1F*) file->Get("pq/h_rej_" + LPFO + LPFO + "_cos");

  // TH1F *h_acc_LPFO_cos_eff_corr = Efficiency_Correction2(h_acc_LPFO_cos,"acc_corr",file);
  // TH1F *h_rej_LPFO_cos_eff_corr = Efficiency_Correction2(h_rej_LPFO_cos,"rej_corr",file);
  TH1F *h_acc_LPFO_cos_eff_corr = (TH1F*) h_acc_LPFO_cos->Clone();
  TH1F *h_rej_LPFO_cos_eff_corr = (TH1F*) h_rej_LPFO_cos->Clone();

  StyleHist(h_gen_q_qcos,kGreen+1);
  h_gen_q_qcos->SetFillStyle(0);
  h_gen_q_qcos->SetLineStyle(2);

  StyleHist(h_cheat_LPFO_qcos,kOrange+1);
  h_cheat_LPFO_qcos->SetFillStyle(0);
  h_cheat_LPFO_qcos->SetLineStyle(2);

  StyleHist(h_reco_LPFO_scos_eff_corr,kBlack);
  h_reco_LPFO_scos_eff_corr->SetFillStyle(0);
  StyleHist(h_reco_LPFO_qcos_eff_corr,kRed+2);
  StyleHist(h_acc_LPFO_cos_eff_corr,kRed+2);
  StyleHist(h_rej_LPFO_cos_eff_corr,kBlue+2);

  const Int_t nbins = h_reco_LPFO_scos_eff_corr->GetNbinsX();

  TH1F *p_LL = new TH1F("p_LL_" + LPFO, "p_LL_" + LPFO, 50,0,1);
  p_LL->Sumw2();

  vector<Float_t> p_vec = GetP(h_acc_LPFO_cos_eff_corr, h_rej_LPFO_cos_eff_corr);

  for (unsigned i = 0; i < p_vec.size() / 2; i++)
  {
    p_LL->SetBinContent(nbins / 2 - i, p_vec.at(i));
    p_LL->SetBinError(nbins / 2 - i, p_vec.at(i + nbins / 2));
  }

  TH1F *h_reco_LPFO_pq_cos = CorrectHist("corrected_" + LPFO, h_reco_LPFO_qcos_eff_corr, p_vec);
  StyleHist(h_reco_LPFO_pq_cos,kBlue);

  // Fitting
  TF1 * f_reco = new TF1("f_reco_" + LPFO,"[0]*(1+x*x)+[1]*x",-0.8,0.8);
  f_reco->SetParNames("S","A");
  h_reco_LPFO_pq_cos->Fit("f_reco_" + LPFO,"MNRS");

  TH1F *h_gen_q_qcos_norm      = Normalize2Gen("h_gen_q_qcos_" + LPFO + "_norm",h_gen_q_qcos,h_reco_LPFO_scos_eff_corr);
  TH1F *h_cheat_LPFO_qcos_norm = Normalize2Gen("h_cheat_" + LPFO + "_norm",h_cheat_LPFO_qcos,h_reco_LPFO_scos_eff_corr);


  TCanvas *c0 = new TCanvas("c0_" + LPFO,"c0_" + LPFO,800,800);
  TPad  *pad0 = new TPad("pad0_" + LPFO, "pad0_" + LPFO,0,0,1,1);
  StylePad(pad0,0,0.12,0,0.15);

  // h_reco_LPFO_pq_cos->GetYaxis()->SetRangeUser(0,50E3);
  h_reco_LPFO_pq_cos->SetTitle(";cos#theta_{" + LPFOLabel[mlpfo] + "^{-}};a.u.");
  h_reco_LPFO_pq_cos->Draw("h");
  h_reco_LPFO_qcos_eff_corr->Draw("hsame");
  h_reco_LPFO_scos_eff_corr->Draw("hsame");
  h_gen_q_qcos_norm->Draw("hsame");
  h_cheat_LPFO_qcos_norm->Draw("hsame");

  f_reco->Draw("same");

  TLegend *leg = new TLegend(0.2,0.76,0.7,0.85);
  leg->SetLineColor(0);
  leg->AddEntry(h_gen_q_qcos_norm,"Generated quark angle","l");
  leg->AddEntry(h_cheat_LPFO_qcos_norm,"Cheated " + LPFOLabel[mlpfo] + "^{-} PFO","l");
  leg->AddEntry(h_reco_LPFO_scos_eff_corr,"Reconstructed " + LPFOLabel[mlpfo] + "^{-} matched with quark angle","l");
  leg->AddEntry(h_reco_LPFO_qcos_eff_corr,"Reconstructed " + LPFOLabel[mlpfo] + "^{-}","l");
  leg->AddEntry(h_reco_LPFO_pq_cos,"Reconstructed " + LPFOLabel[mlpfo] + "^{-} (corrected)","l");
  leg->Draw();


  TCanvas *c1 = new TCanvas("c1_" + LPFO,"c1_" + LPFO,800,800);
  TPad  *pad1 = new TPad("pad0_" + LPFO, "pad0_" + LPFO,0,0,1,1);
  StylePad(pad1,0,0.12,0,0.15);
  
  StyleHist(p_LL,kGreen+2);
  p_LL->SetTitle(";cos#theta_{" + LPFOLabel[mlpfo] + "^{-}};p value");
  p_LL->GetYaxis()->SetRangeUser(0,1);
  p_LL->Draw("h");

  TCanvas *c2 = new TCanvas("c2_" + LPFO ,"c2_" + LPFO,800,800);
  TGaxis::SetMaxDigits(3);
  gPad->SetGrid(1,1);
  h_acc_LPFO_cos_eff_corr->SetTitle(";cos#theta_{" + LPFOLabel[mlpfo] + "^{-}};Entries");

  // h_acc_LPFO_cos_eff_corr->GetYaxis()->SetRangeUser(0,50E3);
  h_acc_LPFO_cos_eff_corr->Draw("h");
  h_rej_LPFO_cos_eff_corr->Draw("hsame");

  TH1F * acc_full = (TH1F*) h_acc_LPFO_cos_eff_corr->Clone();
  acc_full->SetName("acc_full_" + LPFO);
  TH1F * acc_add  = new TH1F("acc_add_" + LPFO,"acc_add_" + LPFO,nbins,-1,1);

  for (int i = 1; i < nbins / 2 + 1; i++)
  {
    float accepted = acc_full->GetBinContent(nbins + 1 - i);
    accepted += acc_full->GetBinContent(i);
    acc_add->SetBinContent(i,accepted);
    acc_add->SetBinContent(nbins+1-i,accepted);
  }
  StyleHist(acc_add,kRed);
  acc_add->Draw("hsame");

  TLegend *leg2 = new TLegend(0.15,0.75,0.45,0.85);
  leg2->SetLineColor(0);
  leg2->AddEntry(h_acc_LPFO_cos_eff_corr,"N Accepted","l");
  leg2->AddEntry(h_rej_LPFO_cos_eff_corr,"N Rejected","l");
  leg2->AddEntry(acc_add,"N Accepted + opp. bin","l");
  leg2->Draw();

}

void main_pq()
{
  gStyle->SetOptStat(0);

  // TFile *file = new TFile("../rootfiles/merged/rv02-02.sv02-02.mILD_l5_o1_v02.E250-SetA.I500010.P2f_z_h.eL.pR.uu.KPiLPFO.PFOp15.LPFOp15_pNaN.tpc0.hists.all.root","READ");

  // TFile *file = new TFile("../rootfiles/merged/rv02-02.sv02-02.mILD_l5_o1_v02.E250-SetA.I500010.P2f_z_h.eL.pR.dd.KPiLPFO.PFOp15.LPFOp15_pNaN.tpc0.hists.all.root","READ");
  
  // TFile *file = new TFile("../rootfiles/merged/rv02-02.sv02-02.mILD_l5_o1_v02.E250-SetA.I500010.P2f_z_h.eL.pR.ss.KPiLPFO.PFOp15.LPFOp15_pNaN.tpc0.hists.all.root","READ");

  // mix
  // TFile *file = new TFile("../rootfiles/merged/rv02-02.sv02-02.mILD_l5_o1_v02.E250-SetA.I500010.P2f_z_h.eL.pR.ud.KPiLPFO.PFOp15.LPFOp15_pNaN.tpc0.hists.all.root","READ");
  TFile *file = new TFile("../rootfiles/merged/rv02-02.sv02-02.mILD_l5_o1_v02.E250-SetA.I500010.P2f_z_h.eL.pR.ud.KPiLPFO.PFOp15.LPFOp15_pNaN.tpc0.hists.all.root","READ");
  // TFile *file = new TFile("../rootfiles/merged/rv02-02.sv02-02.mILD_l5_o1_v02.E250-SetA.I500010.P2f_z_h.eL.pR.uds.KPiLPFO.PFOp15.LPFOp15_pNaN.tpc0.hists.all.root","READ");

  if (!file->IsOpen()) return;

  pq_polar(file, kKaon);
  pq_polar(file, kPion);

}

void pq_method_KPiLPFO()
{
  try
  {
    main_pq();
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }

}